#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

const int MAX_QUEUE_SIZE = 5;
std::queue<int>data_queue;
std::mutex mtx;
std::condition_variable cv;

void Producer(){
	for (int i = 1; i < 10; i++) {
		std::unique_lock<std::mutex>lock(mtx);

		cv.wait(lock, []() {return data_queue.size() < MAX_QUEUE_SIZE; });

		data_queue.push(i);
		std::cout << "生产数据" << i << "队列大小" << data_queue.size() << std::endl;

		lock.unlock();
		cv.notify_one();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void Consumer(int id) {
	while (true)
	{
		std::unique_lock<std::mutex>lock(mtx);
		cv.wait(lock, []() {return !data_queue.empty(); });

		int data = data_queue.front();
		data_queue.pop();
		std::cout << "消费者" << id << "：消费数据" << data << "队列大小" << data_queue.size() << std::endl;

		lock.unlock();
		cv.notify_one();

		if (data == 10) {
			break;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main() {
	std::thread prod(Producer);
	std::thread cons1(Consumer, 1);
	std::thread cons2(Consumer, 2);

	prod.join();
	cons1.join();
	cons2.join();
	return 0;
}