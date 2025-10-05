#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <string>
#include <memory>

class TextEditor {
public:
    virtual void insertChar(char c) = 0;
    virtual void deleteChar() = 0;
    virtual void moveLeft() = 0;
    virtual void moveRight() = 0;
    virtual std::string getTextWithCursor() const = 0;

    virtual ~TextEditor() = default;
};

std::unique_ptr<TextEditor> createTextEditor();

#endif // TEXTEDITOR_H
