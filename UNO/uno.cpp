#include "UNO.h"
#include <random>
#include <algorithm>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <mutex>

// Enumeration for card colors
enum class Color { Red, Green, Blue, Yellow };

// Enumeration for card kinds
enum class Kind { Number, Skip, Reverse, DrawTwo };

// Structure representing a card
struct CardImpl {
    Color color; // Color of the card
    Kind kind;   // Kind/type of the card
    int number;  // Number value (only for number cards)
};

// Function to convert Color enum to string
static std::string colorToString(Color c) {
    switch (c) {
        case Color::Red: return "Red";
        case Color::Green: return "Green";
        case Color::Blue: return "Blue";
        case Color::Yellow: return "Yellow";
    }
    return "";
}

// Function to convert a card to string representation
static std::string cardToStringInternal(const CardImpl& c) {
    std::ostringstream oss;
    oss << colorToString(c.color) << " "; // Add color to string
    if (c.kind == Kind::Number) oss << c.number; // Add number if it's a number card
    else if (c.kind == Kind::Skip) oss << "Skip"; // Add Skip representation
    else if (c.kind == Kind::Reverse) oss << "Reverse"; // Add Reverse representation
    else if (c.kind == Kind::DrawTwo) oss << "DrawTwo"; // Add DrawTwo representation
    return oss.str();
}

// Function to build a deck of UNO cards
static void buildDeck(std::vector<CardImpl>& deck) {
    deck.clear(); // Clear the deck
    const std::array<Color, 4> colors = { Color::Red, Color::Green, Color::Blue, Color::Yellow };
    for (auto col : colors) {
        deck.push_back(CardImpl{col, Kind::Number, 0}); // Add zero card
        for (int v = 1; v <= 9; ++v) {
            deck.push_back(CardImpl{col, Kind::Number, v}); // Add number cards 1-9
            deck.push_back(CardImpl{col, Kind::Number, v}); // Add another instance of the number card
        }
        // Add special cards
        deck.push_back(CardImpl{col, Kind::Skip, -1});
        deck.push_back(CardImpl{col, Kind::Skip, -1});
        deck.push_back(CardImpl{col, Kind::Reverse, -1});
        deck.push_back(CardImpl{col, Kind::Reverse, -1});
        deck.push_back(CardImpl{col, Kind::DrawTwo, -1});
        deck.push_back(CardImpl{col, Kind::DrawTwo, -1});
    }
}

// Structure to hold the game data
struct UNOGameData {
    int numPlayers = 0; // Number of players
    std::vector<std::vector<CardImpl>> hands; // Player hands
    std::vector<CardImpl> deck; // Deck of cards
    std::vector<CardImpl> discard; // Discard pile
    int currentPlayer = 0; // Index of the current player
    int direction = 1; // Direction of play (1 for clockwise, -1 for counter-clockwise)
    int winner = -1; // Index of the winner, -1 if no winner
    std::mt19937 rng; // Random number generator
};

// Global data map for game instances
static std::unordered_map<const UNOGame*, UNOGameData> g_data;
static std::mutex g_data_mutex; // Mutex for thread safety

// Access game data for a given game instance
static UNOGameData& dataFor(UNOGame* g) {
    std::lock_guard<std::mutex> lock(g_data_mutex); // Lock the mutex
    auto it = g_data.find(g);
    if (it == g_data.end()) it = g_data.emplace(g, UNOGameData()).first; // Create new entry if not found
    return it->second; // Return the game data
}

// Access constant game data for a given game instance
static const UNOGameData& dataForConst(const UNOGame* g) {
    std::lock_guard<std::mutex> lock(g_data_mutex); // Lock the mutex
    auto it = g_data.find(g);
    if (it == g_data.end()) {
        static const UNOGameData empty; // Return an empty game data if not found
        return empty;
    }
    return it->second; // Return the game data
}

// Function to draw a card from the deck
static CardImpl drawCardInternal(UNOGameData& d) {
    if (d.deck.empty()) { // Check if the deck is empty
        if (d.discard.size() > 1) { // Reshuffle if the discard pile has cards
            CardImpl top = d.discard.back(); // Get the top card
            std::vector<CardImpl> temp(d.discard.begin(), d.discard.end() - 1); // Temporary storage
            d.discard.clear(); // Clear the discard pile
            d.discard.push_back(top); // Keep the top card
            std::shuffle(temp.begin(), temp.end(), d.rng); // Shuffle the remaining cards
            for (auto &c : temp) d.deck.push_back(c); // Add shuffled cards back to the deck
        }
    }
    if (d.deck.empty()) return CardImpl{Color::Red, Kind::Number, -1}; // Return an empty card if still empty
    CardImpl c = d.deck.back(); // Draw the top card
    d.deck.pop_back(); // Remove the card from the deck
    return c; // Return the drawn card
}

// Function to check if a card can be played on top of another
static bool cardPlayableInternal(const CardImpl& c, const CardImpl& top) {
    if (c.color == top.color) return true; // Same color
    if (c.kind == Kind::Number && top.kind == Kind::Number && c.number == top.number) return true; // Same number
    if (c.kind != Kind::Number && top.kind != Kind::Number && c.kind == top.kind) return true; // Same kind
    return false; // Not playable
}

// Function to find a playable card in hand with priority
static int findPlayableIndexWithPriorityInternal(const std::vector<CardImpl>& hand, const CardImpl& top) {
    // Check for cards matching the top card's color
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].color == top.color && cardPlayableInternal(hand[i], top)) return static_cast<int>(i);
    }
    // Check for cards matching the top card's number or kind
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].kind == Kind::Number && top.kind == Kind::Number && hand[i].number == top.number) return static_cast<int>(i);
        if (hand[i].kind != Kind::Number && top.kind != Kind::Number && hand[i].kind == top.kind) return static_cast<int>(i);
    }
    // Check for special cards (Skip, Reverse, DrawTwo)
    for (auto wanted : { Kind::Skip, Kind::Reverse, Kind::DrawTwo }) {
        for (size_t i = 0; i < hand.size(); ++i) {
            if (hand[i].kind == wanted && cardPlayableInternal(hand[i], top)) return static_cast<int>(i);
        }
    }
    return -1; // No playable cards found
}

// Function to get the next player's index based on direction
static int nextIndexInternal(const UNOGameData& d, int from, int steps = 1) {
    int n = d.numPlayers; // Total number of players
    int s = steps % n; // Steps modulo number of players
    if (s < 0) s += n; // Adjust for negative steps
    int idx = (from + d.direction * s) % n; // Calculate next index
    if (idx < 0) idx += n; // Adjust for negative indices
    return idx; // Return the next index
}

// Constructor for UNOGame
UNOGame::UNOGame(int numPlayers) {
    std::lock_guard<std::mutex> lock(g_data_mutex); // Lock the mutex
    UNOGameData d; // Create game data
    d.numPlayers = (numPlayers < 2 ? 2 : (numPlayers > 4 ? 4 : numPlayers)); // Ensure valid player count
    d.hands.assign(d.numPlayers, {}); // Initialize hands for each player
    d.currentPlayer = 0; // Set current player to the first
    d.direction = 1; // Set direction to clockwise
    d.winner = -1; // No winner yet
    d.rng.seed(1234); // Seed the random number generator
    g_data.emplace(this, std::move(d)); // Store game data
}

// Function to initialize the game
void UNOGame::initialize() {
    UNOGameData& d = dataFor(this); // Access game data
    d.hands.assign(d.numPlayers, {}); // Clear player hands
    d.discard.clear(); // Clear discard pile
    buildDeck(d.deck); // Build and shuffle the deck
    d.rng.seed(1234); // Seed the random number generator
    std::shuffle(d.deck.begin(), d.deck.end(), d.rng); // Shuffle the deck
    for (int r = 0; r < 7; ++r) { // Deal 7 cards to each player
        for (int p = 0; p < d.numPlayers; ++p) {
            if (!d.deck.empty()) {
                d.hands[p].push_back(d.deck.back()); // Give a card to the player
                d.deck.pop_back(); // Remove the card from the deck
            }
        }
    }
    if (!d.deck.empty()) { // If there are cards left, place one on top of the discard pile
        d.discard.push_back(d.deck.back());
        d.deck.pop_back();
    } else {
        d.winner = -2; // No cards left in the deck, game cannot continue
    }
    d.currentPlayer = 0; // Set to first player
    d.direction = 1; // Set direction to clockwise
    d.winner = -1; // Reset winner status
}

// Function to play a turn in the game
void UNOGame::playTurn() {
    UNOGameData& d = dataFor(this); // Access game data
    if (d.winner != -1) return; // Check if game is already won
    if (d.discard.empty()) { d.winner = -2; return; } // No cards in discard pile
    CardImpl top = d.discard.back(); // Get the top card from discard pile
    if (d.deck.empty()) { // If the deck is empty
        bool anyPlayable = false; // Flag to check for playable cards
        for (int p = 0; p < d.numPlayers; ++p) { // Check each player's hand
            for (const auto& c : d.hands[p]) {
                if (cardPlayableInternal(c, top)) { anyPlayable = true; break; } // Found a playable card
            }
            if (anyPlayable) break; // Stop checking if any playable cards were found
        }
        if (!anyPlayable) { d.winner = -2; return; } // No playable cards, game ends
    }
    int p = d.currentPlayer; // Get the current player index
    int chosen = findPlayableIndexWithPriorityInternal(d.hands[p], top); // Find a playable card
    if (chosen != -1) { // If a playable card is found
        CardImpl played = d.hands[p][chosen]; // Get the played card
        d.hands[p].erase(d.hands[p].begin() + chosen); // Remove from hand
        d.discard.push_back(played); // Add to discard pile
        if (d.hands[p].empty()) { d.winner = p; return; } // Player wins if hand is empty
        // Handle special card effects
        if (played.kind == Kind::Skip) {
            d.currentPlayer = nextIndexInternal(d, p, 2); // Skip next player's turn
            return;
        } else if (played.kind == Kind::Reverse) {
            d.direction = -d.direction; // Reverse the direction
            if (d.numPlayers == 2) d.currentPlayer = nextIndexInternal(d, p, 2); // Adjust for 2-player game
            else d.currentPlayer = nextIndexInternal(d, p, 1); // Move to next player
            return;
        } else if (played.kind == Kind::DrawTwo) {
            int victim = nextIndexInternal(d, p, 1); // Determine next player to draw cards
            for (int k = 0; k < 2; ++k) { // Draw two cards for the victim
                CardImpl dc = drawCardInternal(d); // Draw a card
                if (dc.kind == Kind::Number && dc.number == -1) break; // Stop if no cards left
                d.hands[victim].push_back(dc); // Add drawn card to victim's hand
            }
            d.currentPlayer = nextIndexInternal(d, p, 2); // Move to next player's turn
            return;
        } else {
            d.currentPlayer = nextIndexInternal(d, p, 1); // Move to next player
            return;
        }
    } else { // No playable card found
        CardImpl dc = drawCardInternal(d); // Draw a card
        if (!(dc.kind == Kind::Number && dc.number == -1)) { // Check if card drawn is valid
            d.hands[p].push_back(dc); // Add drawn card to current player's hand
            if (cardPlayableInternal(dc, top)) { // Check if the drawn card can be played
                int idx = static_cast<int>(d.hands[p].size()) - 1; // Get index of the drawn card
                CardImpl played = d.hands[p][idx]; // Get the played card
                d.hands[p].erase(d.hands[p].begin() + idx); // Remove from hand
                d.discard.push_back(played); // Add to discard pile
                if (d.hands[p].empty()) { d.winner = p; return; } // Player wins if hand is empty
                // Handle special card effects
                if (played.kind == Kind::Skip) {
                    d.currentPlayer = nextIndexInternal(d, p, 2); // Skip next player's turn
                    return;
                } else if (played.kind == Kind::Reverse) {
                    d.direction = -d.direction; // Reverse the direction
                    if (d.numPlayers == 2) d.currentPlayer = nextIndexInternal(d, p, 2); // Adjust for 2-player game
                    else d.currentPlayer = nextIndexInternal(d, p, 1); // Move to next player
                    return;
                } else if (played.kind == Kind::DrawTwo) {
                    int victim = nextIndexInternal(d, p, 1); // Determine next player to draw cards
                    for (int k = 0; k < 2; ++k) { // Draw two cards for the victim
                        CardImpl d2 = drawCardInternal(d); // Draw a card
                        if (d2.kind == Kind::Number && d2.number == -1) break; // Stop if no cards left
                        d.hands[victim].push_back(d2); // Add drawn card to victim's hand
                    }
                    d.currentPlayer = nextIndexInternal(d, p, 2); // Move to next player's turn
                    return;
                } else {
                    d.currentPlayer = nextIndexInternal(d, p, 1); // Move to next player
                    return;
                }
            }
        }
        d.currentPlayer = nextIndexInternal(d, p, 1); // Move to next player if no valid play was made
        return;
    }
}

// Function to check if the game is over
bool UNOGame::isGameOver() const {
    const UNOGameData& d = dataForConst(this); // Access constant game data
    return d.winner != -1; // Return whether there is a winner
}

// Function to get the winner of the game
int UNOGame::getWinner() const {
    const UNOGameData& d = dataForConst(this); // Access constant game data
    return d.winner; // Return the winner's index
}

// Function to get the current state of the game
std::string UNOGame::getState() const {
    const UNOGameData& d = dataForConst(this); // Access constant game data
    std::ostringstream oss;
    oss << "Player " << d.currentPlayer << "'s turn, Direction: "; // Current player's turn
    oss << (d.direction == 1 ? "Clockwise" : "Counter-clockwise"); // Indicate direction
    if (!d.discard.empty()) oss << ", Top: " << cardToStringInternal(d.discard.back()); // Show top card
    else oss << ", Top: (none)"; // No top card
    oss << ", Players cards: "; // Show card count for each player
    for (int i = 0; i < d.numPlayers; ++i) {
        oss << "P" << i << ":" << d.hands[i].size(); // Show count of cards for player i
        if (i + 1 < d.numPlayers) oss << ", "; // Add separator for players
    }
    return oss.str(); // Return the current state as a string
}