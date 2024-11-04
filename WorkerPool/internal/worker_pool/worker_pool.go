package workerpool

import (
	"fmt"
	"math/rand/v2"
	"sync"
)

type Worker struct {
	id       int
	workChan chan string
	wg       *sync.WaitGroup
	quitChan chan bool
	stop     bool
}

func NewWorker(id int, wg *sync.WaitGroup) *Worker {
	return &Worker{
		id:       id,
		workChan: make(chan string),
		wg:       wg,
		quitChan: make(chan bool),
		stop:     false,
	}
}

func (w *Worker) Start() {
	go func() {
		defer w.wg.Done()

		for {
			select {
			case task := <-w.workChan:
				if w.stop {
					return
				}
				fmt.Printf("Worker %d: %s\n", w.id, task)
			case <-w.quitChan:
				return
			}
		}
	}()
}

func (w *Worker) Stop() {
	w.stop = true
	w.quitChan <- true
}

type WorkerPool struct {
	workers []*Worker
	wg      *sync.WaitGroup
	mutex   sync.Mutex
}

func NewWorkerPool(numWorkers int) *WorkerPool {
	wg := new(sync.WaitGroup)
	wg.Add(numWorkers)
	pool := &WorkerPool{
		workers: make([]*Worker, 0, numWorkers),
		wg:      wg,
		mutex:   sync.Mutex{},
	}

	for i := 0; i < numWorkers; i++ {
		worker := NewWorker(i, wg)
		worker.Start()
		pool.workers = append(pool.workers, worker)
	}

	return pool
}

func (p *WorkerPool) AddWorker() {
	p.mutex.Lock()
	defer p.mutex.Unlock()
	p.wg.Add(1)
	worker := NewWorker(len(p.workers), p.wg)
	worker.Start()
	p.workers = append(p.workers, worker)
}

func (p *WorkerPool) RemoveWorker(id int) {
	p.mutex.Lock()
	defer p.mutex.Unlock()
	for i, worker := range p.workers {
		if worker.id == id {
			worker.Stop()
			p.workers = append(p.workers[:i], p.workers[i+1:]...)
			return
		}
	}
}

func (p *WorkerPool) AddTask(task string) {
	worker := p.workers[rand.IntN(len(p.workers))]
	worker.workChan <- task
}

func (p *WorkerPool) Stop() {
	p.wg.Done()
}
