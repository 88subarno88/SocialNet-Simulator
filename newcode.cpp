#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <queue>
#include <set>
#include <climits>

using namespace std;
struct Post {
    string content;
    long long timestamp;
    
    Post(const string& c, long long t) : content(c), timestamp(t) {}
};

class AVLNode {
public:
    Post post;
    AVLNode* left;
    AVLNode* right;
    int height;
    
    AVLNode(const Post& p) : post(p), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;
    long long postCounter;
    
    int getHeight(AVLNode* node) {
        return node ? node->height : 0;
    }
    
    int getBalance(AVLNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    void updateHeight(AVLNode* node) {
        if (node) {
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        }
    } 
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    AVLNode* insert(AVLNode* node, const Post& post) {
        if (!node) {
            return new AVLNode(post);
        }
        
        if (post.timestamp < node->post.timestamp) {
            node->left = insert(node->left, post);
        } else {
            node->right = insert(node->right, post);
        }
        
        updateHeight(node);
        
        int balance = getBalance(node);
        if (balance > 1 && post.timestamp < node->left->post.timestamp) {
            return rotateRight(node);
        }
        
     
        if (balance < -1 && post.timestamp >= node->right->post.timestamp) {
            return rotateLeft(node);
        }
        if (balance > 1 && post.timestamp >= node->left->post.timestamp) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        if (balance < -1 && post.timestamp < node->right->post.timestamp) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    void inorderReverse(AVLNode* node, vector<Post>& posts) {
        if (!node) return;
        
        inorderReverse(node->right, posts);
        posts.push_back(node->post);
        inorderReverse(node->left, posts);
    }
    
    void deleteTree(AVLNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
public:
    AVLTree() : root(nullptr), postCounter(0) {}
    
    ~AVLTree() {
        deleteTree(root);
    }
    
    void addPost(const string& content) {
        Post post(content, postCounter++);
        root = insert(root, post);
    }
    
    vector<Post> getRecentPosts(int n) {
        vector<Post> allPosts;
        inorderReverse(root, allPosts);
        
        if (n == -1 || n >= (int)allPosts.size()) {
            return allPosts;
        }
        
        if (n <= 0) {
            return vector<Post>();
        }
        
        return vector<Post>(allPosts.begin(), allPosts.begin() + n);
    }
};
class User {
public:
    string username;
    set<string> friends;
    AVLTree posts;
    
    User(const string& name) : username(name) {}
};

class SocialNetwork {
private:
    unordered_map<string, User*> users;
    
    string toLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
public:
    ~SocialNetwork() {
        for (auto& pair : users) {
            delete pair.second;
        }
    }
    
    void addUser(const string& username) {
        string lower = toLower(username);
        if (users.find(lower) != users.end()) {
            cout << "Error: User " << username << " already exists." << endl;
            return;
        }
        
        users[lower] = new User(lower);
    }
    void addFriend(const string& username1, const string& username2) {
        string lower1 = toLower(username1);
        string lower2 = toLower(username2);
        if (lower1 == lower2) {
            cout << "Error: User cannot friend themselves." << endl;
            return;
        }
        bool user1Exists = users.find(lower1) != users.end();
        bool user2Exists = users.find(lower2) != users.end();
        if (!user1Exists && !user2Exists) {
            cout << "Error: Users " << username1 << " and " << username2 << " do not exist." << endl;
            return;
        }
        if (!user1Exists) {
            cout << "Error: User" << username1 << " does not exist." << endl;
            return;
        }
        if (!user2Exists) {
            cout << "Error: User" << username2 << " does not exist." << endl;
            return;
        }
        if (users[lower1]->friends.find(lower2) != users[lower1]->friends.end()) {
            cout << "Error: " << username1 << "and " << username2 << "are already friends." << endl;
            return;
        }
        users[lower1]->friends.insert(lower2);
        users[lower2]->friends.insert(lower1);
    }
    
    void listFriends(const string& username) {
        string lower = toLower(username);
        if (users.find(lower) == users.end()) {
            cout << "Error: User" << username << " does not exist." << endl;
            return;
        }
        vector<string> friendsList(users[lower]->friends.begin(), users[lower]->friends.end());
        sort(friendsList.begin(), friendsList.end());
        for (const string& friendName : friendsList) {
            cout << friendName << endl;
        }
    }
    void suggestFriends(const string& username, int n) {
        string lower = toLower(username);
        if (users.find(lower) == users.end()) {
            cout << "Error: User" << username << " does not exist." << endl;
            return;
        }
        if (n <= 0) {
            return;
        }
        
        User* user = users[lower];
        unordered_map<string, int> mutualCount;
        for (const string& friendName : user->friends) {
            if (users.find(friendName) == users.end()) continue;
            
            User* friendUser = users[friendName];
            for (const string& fofName : friendUser->friends) {
                if (fofName != lower && user->friends.find(fofName) == user->friends.end()) {
                    mutualCount[fofName]++;
                }
            }
        }
        vector<pair<string, int>> candidates;
        for (const auto& pair : mutualCount) {
            candidates.push_back({pair.first, pair.second});
        }
        
        sort(candidates.begin(), candidates.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second != b.second) {
                return a.second > b.second; 
            }
            return a.first < b.first; 
        });
        int count = min(n, (int)candidates.size());
        for (int i = 0; i < count; i++) {
            cout << candidates[i].first << endl;
        }
    }
    void degreesOfSeparation(const string& username1, const string& username2) {
        string lower1 = toLower(username1);
        string lower2 = toLower(username2);
        
        bool user1Exists = users.find(lower1) != users.end();
        bool user2Exists = users.find(lower2) != users.end();
        
        if (!user1Exists && !user2Exists) {
            cout << "Error: Users " << username1 << "and " << username2 << " do not exist." << endl;
            return;
        }
        
        if (!user1Exists) {
            cout << "Error: User " << username1 << " does not exist." << endl;
            return;
        }
        
        if (!user2Exists) {
            cout << "Error: User " << username2 << " does not exist." << endl;
            return;
        }
        
        if (lower1 == lower2) {
            cout << 0 << endl;
            return;
        }
        unordered_map<string, int> distance;
        queue<string> q;
        
        q.push(lower1);
        distance[lower1] = 0;
        
        while (!q.empty()) {
            string current = q.front();
            q.pop();
            if (current == lower2) {
                cout << distance[current] << endl;
                return;
            }
            if (users.find(current) != users.end()) {
                User* currentUser = users[current];
                for (const string& friendName : currentUser->friends) {
                    if (distance.find(friendName) == distance.end()) {
                        distance[friendName] = distance[current] + 1;
                        q.push(friendName);
                    }
                }
            }
        }
        cout << -1 << endl;
    }
    
    void addPost(const string& username, const string& content) {
        string lower = toLower(username);
        if (users.find(lower) == users.end()) {
            cout << "Error: User " << username << " does not exist." << endl;
            return;
        }
        
        users[lower]->posts.addPost(content);
    }
    
    void outputPosts(const string& username, int n) {
        string lower = toLower(username);
        if (users.find(lower) == users.end()) {
            cout << "Error: User" << username << " does not exist." << endl;
            return;
        }
        vector<Post> posts = users[lower]->posts.getRecentPosts(n);
        for (const Post& post : posts) {
            cout << post.content << endl;
        }
    }
};
string extractQuotedString(const string& line, size_t startPos) {
    size_t firstQuote = line.find('"', startPos);
    if (firstQuote == string::npos) {
        return "";
    }
    
    size_t secondQuote = line.find('"', firstQuote + 1);
    if (secondQuote == string::npos) {
        return line.substr(firstQuote + 1);
    }
    
    return line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

void processCommand(SocialNetwork& network, const string& line) {
    if (line.empty()) return;
    
    istringstream iss(line);
    string command, subcommand;
    iss >> command;
    
    if (command == "ADD") {
        iss >> subcommand;
        
        if (subcommand == "USER") {
            string username;
            iss >> username;
            network.addUser(username);
        }
        else if (subcommand == "FRIEND") {
            string username1, username2;
            iss >> username1 >> username2;
            network.addFriend(username1, username2);
        }
        else if (subcommand == "POST") {
            string username;
            iss >> username;
            string content = extractQuotedString(line, line.find(username) + username.length());
            network.addPost(username, content);
        }
    }
    else if (command == "LIST") {
        iss >> subcommand;
        
        if (subcommand == "FRIENDS") {
            string username;
            iss >> username;
            network.listFriends(username);
        }
    }
    else if (command == "SUGGEST") {
        iss >> subcommand;
        
        if (subcommand == "FRIENDS") {
            string username;
            int n;
            iss >> username >> n;
            network.suggestFriends(username, n);
        }
    }
    else if (command == "DEGREES") {
        string of, separation, username1, username2;
        iss >> of >> separation >> username1 >> username2;
        network.degreesOfSeparation(username1, username2);
    }
    else if (command == "OUTPUT") {
        iss >> subcommand;
        
        if (subcommand == "POSTS") {
            string username;
            int n;
            iss >> username >> n;
            network.outputPosts(username, n);
        }
    }
}

int main() {
    SocialNetwork network;
    string line;
    
    while (getline(cin, line)) {
        processCommand(network, line);
    }
    
    return 0;
}