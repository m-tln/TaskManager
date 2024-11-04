package main

import (
	"fmt"
	worker_pool "worker_pool/internal/worker_pool"
)

func main() {
	wp := *worker_pool.NewWorkerPool(3)
	defer wp.Stop()

	taskChan := make(chan string)

	go func() {
		for i := 0; i < 10; i++ {
			taskChan <- fmt.Sprintf("Task %d", i)
		}
		close(taskChan)
	}()

	for task := range taskChan {
		wp.AddTask(task)
	}
	wp.AddWorker()
	wp.RemoveWorker(1)
	wp.AddTask("New task")
}
