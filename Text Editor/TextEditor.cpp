#include "TextEditor.h"

#include <deque>
#include <string>
#include <memory>
#include <sstream>
#include <algorithm>

// Implementation class - keeps the actual editor logic hidden
class ConcreteTextEditor : public TextEditor {
public:
    ConcreteTextEditor() = default;
    ~ConcreteTextEditor() override = default;

    void insertChar(char character) override {
        // Just add the character to the left side (before cursor)
        leftSide_.push_back(character);
    }

    void deleteChar() override {
        // Only delete if there's something to delete on the left
        if (!leftSide_.empty()) {
            leftSide_.pop_back();
        }
    }

    void moveLeft() override {
        // Move cursor left by transferring character from left to right
        if (!leftSide_.empty()) {
            char ch = leftSide_.back();
            leftSide_.pop_back();
            rightSide_.push_front(ch);
        }
    }

    void moveRight() override {
        // Move cursor right by transferring character from right to left  
        if (!rightSide_.empty()) {
            char ch = rightSide_.front();
            rightSide_.pop_front();
            leftSide_.push_back(ch);
        }
    }

    std::string getTextWithCursor() const override {
        std::string result;
        
        // Pre-allocate space for efficiency (though probably not necessary for most cases)
        result.reserve(leftSide_.size() + rightSide_.size() + 1);
        
        // Add all characters from left side
        for (char ch : leftSide_) {
            result.push_back(ch);
        }
        
        // Add cursor marker
        result.push_back('|');
        
        // Add all characters from right side
        for (char ch : rightSide_) {
            result.push_back(ch);
        }
        
        return result;
    }

private:
    std::deque<char> leftSide_;  // Characters to the left of cursor
    std::deque<char> rightSide_; // Characters to the right of cursor
    
    // Note: Using deque because we need efficient insertion/deletion at both ends
    // Could also use vector but deque is probably better for this use case
};

// Factory function to create text editor instances
std::unique_ptr<TextEditor> createTextEditor() {
    return std::make_unique<ConcreteTextEditor>();
}