#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <Windows.h>

std::atomic<int> client_counter(0);
std::atomic<bool> finished(false);

void client(int max_clients, std::memory_order order) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int current_counter = client_counter.load(order);
        if (current_counter < max_clients) {
            client_counter.fetch_add(1, order);
            std::cout << "Новый клиент добавлен. Клиентов в очереди: " << client_counter.load(order) << std::endl;
        }
        if (finished.load(order)) break;
    }
}

void operator_thread(std::memory_order order) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        int current_counter = client_counter.load(order);
        if (current_counter > 0) {
            client_counter.fetch_sub(1, order);
            std::cout << "Клиент обслужен. Клиентов в очереди: " << client_counter.load(order) << std::endl;
        }
        else if (finished.load(order)) {
            break;
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    int max_clients;
    std::cout << "Введите максимальное количество клиентов: ";
    std::cin >> max_clients;

    
    std::memory_order order = std::memory_order_seq_cst;  

    std::thread client_thread(client, max_clients, order);
    std::thread operator_thread_instance(operator_thread, order);

    std::this_thread::sleep_for(std::chrono::seconds(max_clients * 2 + 5)); 

    finished.store(true, order);

    client_thread.join();
    operator_thread_instance.join();

    std::cout << "Все клиенты обслужены." << std::endl;
    return 0;
}
