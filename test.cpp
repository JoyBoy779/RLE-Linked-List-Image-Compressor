#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <functional> // Required for std::function used in performOperation

using namespace std;

// --- Custom Exception ---
class BoundsMismatchException : public runtime_error {
public:
    BoundsMismatchException(const string& message) : runtime_error(message) {}
};

// --- Compressed Linked List Structure ---
// Represents a single run of BLACK (0) pixels: [start, end]
struct Node {
    int start_index;
    int end_index;
    Node* next;

    Node(int start, int end) : start_index(start), end_index(end), next(nullptr) {}
};

// --- Image Interface ---
class CompressedImageInterface {
public:
    virtual ~CompressedImageInterface() = default;
    virtual void performAnd(CompressedImageInterface* img) = 0;
    virtual void performXor(CompressedImageInterface* img) = 0;
    virtual void invert() = 0;
    // FIX 1: Add toStringCompressed to the interface so it can be called polymorphically
    virtual string toStringCompressed() = 0;
};

// Main Image Class
class RunLengthImage : public CompressedImageInterface {
private:
    vector<Node*> image; // Vector of head pointers, one for each row
    int height;
    int width;

    // Helper to free memory for a single row
    void cleanup_row(int i) {
        Node* current = image[i];
        while (current != nullptr) {
            Node* next_node = current->next;
            delete current;
            current = next_node;
        }
        image[i] = nullptr;
    }

    // Helper to convert an image row (linked list) into a simple 1D boolean grid
    vector<bool> rowToGrid(int i) {
        if (i < 0 || i >= height) throw out_of_range("Row index out of range.");
        vector<bool> row(width, true); // Initialize to White (1)
        Node* current = image[i];

        while (current != nullptr) {
            for (int j = current->start_index; j <= current->end_index; ++j) {
                row[j] = false; // Set Black (0) for the run
            }
            current = current->next;
        }
        return row;
    }

    // Helper to convert a 1D boolean grid back into a compressed linked list row
    void reconstructRow(int i, const vector<bool>& row) {
        // NOTE: cleanup_row(i) is handled in the calling function (performOperation/invert)
        
        Node* head = nullptr;
        Node* tail = nullptr;
        
        bool in_black_run = false;
        int current_start = -1;

        for (int j = 0; j < width; ++j) {
            if (!row[j]) { // Black pixel (0)
                if (!in_black_run) {
                    current_start = j;
                    in_black_run = true;
                }
            } else { // White pixel (1)
                if (in_black_run) {
                    Node* new_node = new Node(current_start, j - 1);
                    if (head == nullptr) head = new_node;
                    if (tail != nullptr) tail->next = new_node;
                    tail = new_node;
                    in_black_run = false;
                }
            }
        }

        // Handle run ending at the last pixel
        if (in_black_run) {
            Node* new_node = new Node(current_start, width - 1);
            if (head == nullptr) head = new_node;
            if (tail != nullptr) tail->next = new_node;
        }

        image[i] = head;
    }

public:
    RunLengthImage(const vector<vector<int>>& grid, int w, int h) : width(w), height(h) {
        image.resize(h);
        for (int i = 0; i < h; i++) {
            // This loop performs the compression (CV Claim 2: Pixel grouping)
            Node* head = nullptr;
            Node* tail = nullptr;
            int start = -1;
            
            for (int j = 0; j < w; j++) {
                if (grid[i][j] == 0) { // Black pixel
                    if (start == -1) {
                        start = j;
                    }
                } else { // White pixel
                    if (start != -1) {
                        Node* new_node = new Node(start, j - 1);
                        if (head == nullptr) head = new_node;
                        if (tail != nullptr) tail->next = new_node;
                        tail = new_node;
                        start = -1;
                    }
                }
            }
            // Check for a black run at the end of the row
            if (start != -1) {
                Node* new_node = new Node(start, w - 1);
                if (head == nullptr) head = new_node;
                if (tail != nullptr) tail->next = new_node;
            }
            image[i] = head;
        }
    }
    
    ~RunLengthImage() override {
        for (int i = 0; i < height; ++i) {
            cleanup_row(i);
        }
    }

    // --- CV Claim 3: Boolean Operations ---
    
    // Simplifies implementation by using the array/reconstruct pattern
    void performOperation(CompressedImageInterface* img, function<bool(bool, bool)> op) {
        RunLengthImage* other = dynamic_cast<RunLengthImage*>(img);
        if (other == nullptr || this->width != other->width || this->height != other->height) {
            throw BoundsMismatchException("Size of the two images do not match!");
        }

        for (int i = 0; i < height; i++) {
            vector<bool> row1 = rowToGrid(i); // Decompress first image
            vector<bool> row2 = other->rowToGrid(i); // Decompress second image
            
            for (int j = 0; j < width; j++) {
                row1[j] = op(row1[j], row2[j]);
            }
            
            cleanup_row(i); // Clean up the old linked list for this row
            reconstructRow(i, row1); // Build the new linked list for this row
        }
    }

    void performAnd(CompressedImageInterface* img) override {
        performOperation(img, [](bool a, bool b){ return a && b; });
    }

    void performOr(CompressedImageInterface* img) {
        performOperation(img, [](bool a, bool b){ return a || b; });
    }

    void performXor(CompressedImageInterface* img) override {
        performOperation(img, [](bool a, bool b){ return a ^ b; });
    }
    
    void invert() override {
        for (int i = 0; i < height; i++) {
            vector<bool> row = rowToGrid(i);
            // Fix for std::vector<bool> iteration issue: use index-based loop
            for (int j = 0; j < width; j++) {
                row[j] = !row[j];
            }
            cleanup_row(i); // Clean up before reconstruction
            reconstructRow(i, row);
        }
    }

    // Implementation of the virtual function
    string toStringCompressed() override {
        stringstream ss;
        ss << width << " " << height << ", ";
        for (int i = 0; i < height; ++i) {
            Node* current = image[i];
            if (current == nullptr) {
                ss << " / "; // Represent all-white row
            } else {
                while (current != nullptr) {
                    ss << "(" << current->start_index << "," << current->end_index << ") ";
                    current = current->next;
                }
            }
            ss << ",";
        }
        string result = ss.str();
        // Remove trailing comma and space
        if (result.length() > 2) {
            return result.substr(0, result.length() - 1);
        }
        return result;
    }
};

// Helper function to convert raw string data into a 2D grid
vector<vector<int>> parseImageString(const string& raw_data, int& w, int& h) {
    stringstream ss(raw_data);
    
    // Read dimensions (w, h)
    ss >> w >> h;
    
    vector<vector<int>> grid(h, vector<int>(w));
    
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            // Read 0 or 1 for pixel value
            ss >> grid[i][j];
        }
    }
    return grid;
}


int main() {
    // Large 16x16 image data (from your previous input)
    const string RAW_IMAGE_DATA = 
        "16 16\n"
        "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n"
        "1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 1\n"
        "1 1 1 0 0 0 0 0 1 1 1 1 1 1 1 1\n"
        "1 1 0 0 0 0 0 0 1 1 1 1 1 1 1 1\n"
        "1 1 0 1 1 1 0 0 1 1 1 1 1 1 1 1\n"
        "1 1 1 1 1 1 0 0 1 1 1 1 1 1 1 1\n"
        "1 1 1 1 1 1 0 0 1 1 1 1 1 1 1 1\n"
        "1 1 1 1 0 0 0 1 1 1 1 1 1 1 1 1\n"
        "1 1 0 0 0 1 1 1 1 1 1 1 1 1 1 1\n"
        "1 1 0 0 1 1 1 1 1 1 1 1 1 1 0 0\n"
        "1 1 0 1 1 1 1 1 1 1 1 1 1 0 0 0\n"
        "1 1 1 1 1 1 1 1 1 1 1 0 0 0 1 1\n"
        "1 1 1 1 1 1 1 1 1 1 1 0 0 1 1 1\n"
        "1 1 1 1 1 1 1 1 1 1 0 0 1 1 1 1\n"
        "1 1 1 1 1 1 1 1 1 0 0 1 1 1 1 1\n"
        "1 1 1 1 1 1 1 0 0 0 1 1 1 1 1 1";

    // FIX 2: Declare w and h before use
    int w = 0, h = 0;
    vector<vector<int>> initial_grid = parseImageString(RAW_IMAGE_DATA, w, h);

    cout << "--- Initializing 16x16 Compressed Images ---" << endl;

    // Img1: The original image
    unique_ptr<CompressedImageInterface> img1 = make_unique<RunLengthImage>(initial_grid, w, h);
    cout << "Img1 Compressed (Initial): " << img1->toStringCompressed() << "\n\n";

    // Img2: A copy that will be inverted
    unique_ptr<CompressedImageInterface> img2 = make_unique<RunLengthImage>(initial_grid, w, h);
    img2->invert();
    cout << "Img2 Compressed (Inverted): " << img2->toStringCompressed() << "\n\n";
    
    // Test 1: XOR Operation
    cout << "--- Testing XOR (Img1 ^ Img2) ---" << endl;
    cout << "Expected Result: All white (16 / characters)" << endl;
    img1->performXor(img2.get());
    cout << "Img1 after XOR: " << img1->toStringCompressed() << "\n\n";

    // Test 2: AND Operation
    cout << "--- Testing AND (Img1(White) & Img2(Inverted)) ---" << endl;
    cout << "Expected Result: Should match Img2's original inverted state" << endl;
    img1->performAnd(img2.get());
    cout << "Img1 after AND: " << img1->toStringCompressed() << endl;

    cout << "\nAll boolean operations completed successfully." << endl;

    return 0;
}
