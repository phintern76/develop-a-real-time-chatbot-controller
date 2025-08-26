#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// Chatbot Controller Class
class ChatbotController {
private:
    map<string, string> intents;
    mutex mtx;
    condition_variable cv;
    bool isRunning;
    thread workerThread;

    // Process incoming message
    void processMessage(string message) {
        string response = "I didn't understand that.";
        for (auto& intent : intents) {
            if (message.find(intent.first) != string::npos) {
                response = intent.second;
                break;
            }
        }
        cout << "Bot: " << response << endl;
    }

    // Worker thread function
    void workerFunction() {
        string message;
        while (isRunning) {
            {
                unique_lock<mutex> lock(mtx);
                cv.wait(lock);
                cout << "User: ";
                getline(cin, message);
            }
            processMessage(message);
        }
    }

public:
    ChatbotController() : isRunning(true) {
        // Initialize intents
        intents["hello"] = "Hi! How can I assist you today?";
        intents["goodbye"] = "Goodbye! It was nice chatting with you.";
        intents["help"] = "I can assist you with any questions you have.";

        // Start worker thread
        workerThread = thread(&ChatbotController::workerFunction, this);
    }

    // Add intent to chatbot
    void addIntent(string keyword, string response) {
        intents[keyword] = response;
    }

    // Send message to chatbot
    void sendMessage(string message) {
        {
            lock_guard<mutex> lock(mtx);
            cv.notify_one();
        }
    }

    // Stop chatbot
    void stop() {
        isRunning = false;
        sendMessage(""); // Send empty message to wake up worker thread
        workerThread.join();
    }
};

int main() {
    ChatbotController chatbot;

    // Add custom intents
    chatbot.addIntent("what is your name", "My name is VDF5.");

    // Start chatbot
    chatbot.sendMessage(""); // Send initial message to start chatbot

    // Stop chatbot after 10 seconds
    this_thread::sleep_for(chrono::seconds(10));
    chatbot.stop();

    return 0;
}