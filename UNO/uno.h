#ifndef UNO_H
#define UNO_H

#include <string>
#include <vector>

struct Card {
    
};

class UNOGame {
public:
    UNOGame(int numPlayers);
    virtual void initialize();
    virtual void playTurn();
    virtual bool isGameOver() const;
    virtual int getWinner() const;
    virtual std::string getState() const;
};

#endif
