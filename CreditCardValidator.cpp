#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <deque>
#include <chrono>

using namespace std;

struct CreditCard {
    string number;
    string cardType;
    string expirationDate;
    string cvv;
};

bool isLuhnValid(const string& cardNumber) {
    int sum = 0;
    bool doubleDigit = false;

    for (int i = cardNumber.length() - 1; i >= 0; i--) {
        int digit = cardNumber[i] - '0';

        if (doubleDigit) {
            digit *= 2;
            if (digit > 9) {
                digit = digit % 10 + digit / 10;
            }
        }

        sum += digit;
        doubleDigit = !doubleDigit;
    }

    return (sum % 10 == 0);
}

bool validateExpirationDate(const string& expDate) {
    // Get the current date and time
    auto now = chrono::system_clock::now();
    auto time_t_now = chrono::system_clock::to_time_t(now);
    tm currentTime = *gmtime(&time_t_now);
    int currentYear = currentTime.tm_year + 1900;
    int currentMonth = currentTime.tm_mon + 1;

    if (expDate.length() != 5 || expDate[2] != '/') {
        return false;
    }

    int expYear, expMonth;
    if (sscanf(expDate.c_str(), "%d/%d", &expMonth, &expYear) != 2) {
        return false;
    }

    // Convert two-digit year to a full year
    if (expYear >= 0 && expYear <= 49) {
        expYear += 2000;
    } else if (expYear >= 50 && expYear <= 99) {
        expYear += 1900;
    } else {
        // Handle invalid year values
        return false;
    }

    if (expYear < currentYear || (expYear == currentYear && expMonth < currentMonth)) {
        return false;
    }

    return true;
}

bool isCardLengthValid(const string& cardNumber) {
    int length = cardNumber.length();
    return (length >= 14 && length <= 16); // Accept 14, 15, and 16 digit cards
}

bool compareLastFourDigits(const CreditCard& card1, const CreditCard& card2) {
    // Custom comparison function for sorting by last four digits
    string lastFour1 = card1.number.substr(card1.number.length() - 4);
    string lastFour2 = card2.number.substr(card2.number.length() - 4);
    return lastFour1 < lastFour2;
}

int main() {
    deque<CreditCard> creditCards;
    map<string, string> cardTypes;
    map<string, int> cvvLengths;  // Map card type to CVV length
    unordered_map<string, int> cardTypeFrequency;  // Track card type frequency
    multimap<string, CreditCard> cardTypeMap;  // Multimap to sort by card type
    unordered_set<string> uniqueCardNumbers;  // Track unique card numbers

    cardTypes["4"] = "Visa";
    cardTypes["34"] = "American Express";
    cardTypes["37"] = "American Express";
    cardTypes["51"] = "MasterCard";
    cardTypes["52"] = "MasterCard";
    cardTypes["53"] = "MasterCard";
    cardTypes["54"] = "MasterCard";
    cardTypes["55"] = "MasterCard";
    cvvLengths["Visa"] = 3;
    cvvLengths["American Express"] = 4;
    cvvLengths["MasterCard"] = 3;

    while (true) {
        CreditCard newCard;
        cout << "Enter Credit Card Number (or type 'exit' to quit): ";
        cin >> newCard.number;

        if (newCard.number == "exit") {
            break;
        }

        // Check for duplicate card numbers
        if (uniqueCardNumbers.find(newCard.number) != uniqueCardNumbers.end()) {
            cout << "Duplicate card number. Please enter a unique credit card number." << endl;
            continue;
        }

        uniqueCardNumbers.insert(newCard.number);

        if (!isCardLengthValid(newCard.number)) {
            cout << "Invalid card number length. Please enter a valid credit card number with 14, 15, or 16 digits." << endl;
            continue;
        }

        for (int i = 0; i < newCard.number.length(); i++) {
            char c = newCard.number[i];
            if (!isdigit(c)) {
                cout << "Invalid character in the card number. Please enter a valid credit card number." << endl;
                break;
            }
        }

        if (!isLuhnValid(newCard.number)) {
            cout << "Invalid credit card number (Luhn check failed). Please enter a valid credit card number." << endl;
            continue;
        }

        cout << "Enter Expiration Date (MM/YY): ";
        cin >> newCard.expirationDate;

        if (validateExpirationDate(newCard.expirationDate)) {
            newCard.cardType = "Other Card Type";
            for (auto it = cardTypes.begin(); it != cardTypes.end(); ++it) {
                if (newCard.number.find(it->first) == 0) {
                    newCard.cardType = it->second;
                    break;
                }
            }

            // Check CVV length based on card type
            if (cvvLengths.find(newCard.cardType) != cvvLengths.end()) {
                cout << "Enter CVV (" << cvvLengths[newCard.cardType] << " digits): ";
                cin >> newCard.cvv;
                if (newCard.cvv.length() != cvvLengths[newCard.cardType] || !isdigit(newCard.cvv[0])) {
                    cout << "Invalid CVV length or format. Please enter a valid CVV." << endl;
                    continue;
                }
            } else {
                cout << "Invalid card type. CVV validation skipped." << endl;
            }

            // Store the credit card details in the deque
            creditCards.push_back(newCard);

            // Update card type frequency
            cardTypeFrequency[newCard.cardType]++;

            // Update multimap for sorting by card type
            cardTypeMap.insert(make_pair(newCard.cardType, newCard));

            cout << "Credit Card added successfully." << endl;
        } else {
            cout << "Invalid Expiration Date. Credit Card not added." << endl;
        }
    }

    // Sort credit cards by last four digits using custom comparison function
    sort(creditCards.begin(), creditCards.end(), compareLastFourDigits);

    // Display the sorted credit cards
    cout << "\nSorted Credit Cards by Last Four Digits:\n";
    for (deque<CreditCard>::iterator it = creditCards.begin(); it != creditCards.end(); ++it) {
        const CreditCard& card = *it;
        cout << "Card Type: " << card.cardType << ", Last Four Digits: " << card.number.substr(card.number.length() - 4) << endl;
    }

    // Display card type frequencies
    cout << "\nCard Type Frequencies:\n";
    for (unordered_map<string, int>::iterator it = cardTypeFrequency.begin(); it != cardTypeFrequency.end(); ++it) {
        cout << it->first << ": " << it->second << " cards" << endl;
    }

    // Display credit cards sorted by card type
    cout << "\nCredit Cards Sorted by Card Type:\n";
    for (multimap<string, CreditCard>::iterator it = cardTypeMap.begin(); it != cardTypeMap.end(); ++it) {
        const CreditCard& card = it->second;
        cout << "Card Type: " << it->first << ", Last Four Digits: " << card.number.substr(card.number.length() - 4) << endl;
    }

    return 0;
}
