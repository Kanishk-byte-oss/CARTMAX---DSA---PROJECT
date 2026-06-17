/*
===========================================
 CARTMAX E-COMMERCE SYSTEM
 Data Structures Project

 Data Structures Used:
 1. Trie -> Promo Code Lookup
 2. Stack -> Undo Cart Changes
 3. Queue -> Checkout Processing
 4. Hash Map -> Membership & Inventory
 5. Priority Queue -> Cart Monitoring
 6. Graph -> Product Recommendations
 7. Dynamic Programming -> Best Discount
 8. Hash Map -> Stock Reservation

===========================================
*/

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <stack>
#include <queue>
#include <algorithm>

using namespace std;

// ================= PRODUCT =================

struct Product
{
    string id;
    string name;
    double price;
    int stock;

    Product() {}

    Product(
        string pid,
        string pname,
        double pprice,
        int pstock)
    {
        id = pid;
        name = pname;
        price = pprice;
        stock = pstock;
    }
};

// ================= COUPON =================

struct Coupon
{
    string code;
    bool isPercentage;
    double value;
};

// ================= CART ITEM =================

struct CartItem
{
    Product product;
    int quantity;
};

// ================= TRIE =================

struct TrieNode
{
    unordered_map<char, TrieNode*> children;
    bool isEndOfCode = false;
};

// ================= CARTMAX SYSTEM =================

class CartMaxSystem
{
private:

    // Promo Code Lookup
    TrieNode* promoRoot;

    // Undo Price Changes
    stack<double> priceHistory;

    double currentCartTotal;

    // Checkout Queue
    queue<string> checkoutLine;

    // Membership Database
    unordered_map<string, string> membershipDB;

    // Cart Monitoring Heap
    priority_queue<pair<double, string>> cartSorter;

    // Product Recommendation Graph
    unordered_map<string, vector<string>> productGraph;

    // Product Database + Inventory
    unordered_map<string, Product> productDB;

    // Customer Cart
    vector<CartItem> cart;

    // Order History
    vector<string> orderHistory;

public:

    CartMaxSystem()
    {
        promoRoot = new TrieNode();

        currentCartTotal = 0.0;

        priceHistory.push(0.0);
    }

    ~CartMaxSystem()
    {
        deleteTrie(promoRoot);
    }

private:

    void deleteTrie(TrieNode* node)
    {
        if(node == nullptr)
            return;

        for(auto &child : node->children)
        {
            deleteTrie(child.second);
        }

        delete node;
    }

public:

    // ===== Method declarations =====

    // Trie
    void addPromoCode(string code);
    vector<string> getPromoSuggestions(string prefix);
    void collectSuggestions(
        TrieNode* node,
        string current,
        vector<string>& result);

    // Products
    void addProduct(
        string id,
        string name,
        double price,
        int stock);

    void displayProducts();

    // Inventory
    bool reserveItem(
        string productId,
        int quantity);

    void releaseItem(
        string productId,
        int quantity);

    // Cart
    void addToCart(
        string productId,
        int quantity);

    void removeFromCart(
        string productId);

    void viewCart();

    double getCartTotal();

    // Stack Undo
    void undoLastPriceChange();

    // Membership
    void addMember(
        string customerId,
        string tier);

    string checkMembership(
        string customerId);

    // Queue
    void addToCheckoutProcess(
        string customerId);

    void processNextInLine();

    void displayCheckoutQueue();

    // Heap
    void logCartForMonitoring(
        string customerId,
        double totalValue);

    void getHighestValueCart();

    void displayTop3Carts();

    // Graph
    void addBundleRelationship(
        string productA,
        string productB);

    void getRecommendations(
        string product);

    // Dynamic Programming
    double applyBestDiscount(
        vector<Coupon> coupons);

    // Order History
    void showOrderHistory();
}; 
// =====================================================
// TRIE IMPLEMENTATION
// =====================================================

void CartMaxSystem::addPromoCode(string code)
{
    TrieNode* current = promoRoot;

    for(char c : code)
    {
        if(!current->children.count(c))
        {
            current->children[c] = new TrieNode();
        }

        current = current->children[c];
    }

    current->isEndOfCode = true;
}

void CartMaxSystem::collectSuggestions(
    TrieNode* node,
    string current,
    vector<string>& result)
{
    if(node->isEndOfCode)
    {
        result.push_back(current);
    }

    for(auto &child : node->children)
    {
        collectSuggestions(
            child.second,
            current + child.first,
            result
        );
    }
}

vector<string> CartMaxSystem::getPromoSuggestions(
    string prefix)
{
    TrieNode* current = promoRoot;

    for(char c : prefix)
    {
        if(!current->children.count(c))
        {
            return {};
        }

        current = current->children[c];
    }

    vector<string> result;

    collectSuggestions(
        current,
        prefix,
        result
    );

    return result;
}

// =====================================================
// PRODUCTS
// =====================================================

void CartMaxSystem::addProduct(
    string id,
    string name,
    double price,
    int stock)
{
    productDB[id] =
    Product(
        id,
        name,
        price,
        stock
    );
}

void CartMaxSystem::displayProducts()
{
    cout << "\n===== PRODUCTS =====\n";

    for(auto &p : productDB)
    {
        cout
        << p.second.id
        << " | "
        << p.second.name
        << " | Rs."
        << p.second.price
        << " | Stock: "
        << p.second.stock
        << endl;
    }
}

// =====================================================
// INVENTORY
// =====================================================

bool CartMaxSystem::reserveItem(
    string productId,
    int quantity)
{
    if(productDB.find(productId)
       == productDB.end())
    {
        return false;
    }

    if(productDB[productId].stock
       >= quantity)
    {
        productDB[productId].stock
        -= quantity;

        return true;
    }

    return false;
}

void CartMaxSystem::releaseItem(
    string productId,
    int quantity)
{
    if(productDB.find(productId)
       != productDB.end())
    {
        productDB[productId].stock
        += quantity;
    }
}

// =====================================================
// CART
// =====================================================

void CartMaxSystem::addToCart(
    string productId,
    int quantity)
{
    if(!reserveItem(
        productId,
        quantity))
    {
        cout
        << "\nInsufficient Stock\n";

        return;
    }

    Product p =
    productDB[productId];

    bool found = false;

    for(auto &item : cart)
    {
        if(item.product.id
           == productId)
        {
            item.quantity += quantity;
            found = true;
            break;
        }
    }

    if(!found)
    {
        CartItem item;

        item.product = p;
        item.quantity = quantity;

        cart.push_back(item);
    }

    currentCartTotal +=
    (p.price * quantity);

    priceHistory.push(
        currentCartTotal
    );

    cout
    << "\nItem Added Successfully\n";
}

void CartMaxSystem::removeFromCart(
    string productId)
{
    for(auto it = cart.begin();
        it != cart.end();
        ++it)
    {
        if(it->product.id
           == productId)
        {
            releaseItem(
                productId,
                it->quantity
            );

            currentCartTotal -=
            (
                it->product.price *
                it->quantity
            );

            priceHistory.push(
                currentCartTotal
            );

            cart.erase(it);

            cout
            << "\nItem Removed\n";

            return;
        }
    }

    cout
    << "\nProduct Not Found\n";
}

void CartMaxSystem::viewCart()
{
    cout
    << "\n===== CART =====\n";

    if(cart.empty())
    {
        cout
        << "Cart Empty\n";

        return;
    }

    for(auto &item : cart)
    {
        cout
        << item.product.name
        << " | Qty: "
        << item.quantity
        << " | Rs."
        << item.product.price
        << endl;
    }

    cout
    << "\nTotal: Rs."
    << currentCartTotal
    << endl;
}

double CartMaxSystem::getCartTotal()
{
    return currentCartTotal;
}

// =====================================================
// STACK UNDO
// =====================================================

void CartMaxSystem::undoLastPriceChange()
{
    if(priceHistory.size()
       <= 1)
    {
        cout
        << "\nNothing To Undo\n";

        return;
    }

    priceHistory.pop();

    currentCartTotal =
    priceHistory.top();

    cout
    << "\nUndo Successful\n";
}

// =====================================================
// MEMBERSHIP
// =====================================================

void CartMaxSystem::addMember(
    string customerId,
    string tier)
{
    membershipDB[customerId]
    = tier;
}

string CartMaxSystem::checkMembership(
    string customerId)
{
    if(
      membershipDB.find(
      customerId)
      != membershipDB.end())
    {
        return membershipDB[
               customerId];
    }

    return "Standard";
}

// =====================================================
// QUEUE
// =====================================================

void CartMaxSystem::addToCheckoutProcess(
    string customerId)
{
    checkoutLine.push(
        customerId
    );
}

void CartMaxSystem::displayCheckoutQueue()
{
    queue<string> temp =
    checkoutLine;

    cout
    << "\n===== CHECKOUT QUEUE =====\n";

    if(temp.empty())
    {
        cout
        << "Queue Empty\n";

        return;
    }

    while(!temp.empty())
    {
        cout
        << temp.front()
        << endl;

        temp.pop();
    }
}

void CartMaxSystem::processNextInLine()
{
    if(checkoutLine.empty())
    {
        cout
        << "\nQueue Empty\n";

        return;
    }

    cout
    << "\nProcessing Customer: "
    << checkoutLine.front()
    << endl;

    orderHistory.push_back(
        checkoutLine.front()
    );

    checkoutLine.pop();
}

// =====================================================
// PRIORITY QUEUE (HEAP)
// =====================================================

void CartMaxSystem::logCartForMonitoring(
    string customerId,
    double totalValue)
{
    cartSorter.push(
        {
            totalValue,
            customerId
        }
    );
}

void CartMaxSystem::getHighestValueCart()
{
    if(cartSorter.empty())
    {
        cout
        << "\nNo Cart Data\n";

        return;
    }

    auto top =
    cartSorter.top();

    cout
    << "\nHighest Value Cart\n";

    cout
    << "Customer: "
    << top.second
    << endl;

    cout
    << "Value: Rs."
    << top.first
    << endl;
}

// =====================================================
// GRAPH
// =====================================================

void CartMaxSystem::addBundleRelationship(
    string productA,
    string productB)
{
    productGraph[productA]
        .push_back(productB);

    productGraph[productB]
        .push_back(productA);
}

void CartMaxSystem::getRecommendations(
    string product)
{
    cout
    << "\nRecommendations:\n";

    if(productGraph.find(product)
       == productGraph.end())
    {
        cout
        << "No Recommendations\n";

        return;
    }

    for(auto &item :
        productGraph[product])
    {
        cout
        << "- "
        << item
        << endl;
    }
}
// =====================================================
// TOP 3 CARTS
// =====================================================

void CartMaxSystem::displayTop3Carts()
{
    if(cartSorter.empty())
    {
        cout << "\nNo Cart Data\n";
        return;
    }

    priority_queue<pair<double,string>> temp =
    cartSorter;

    cout << "\n===== TOP 3 CARTS =====\n";

    int count = 0;

    while(!temp.empty() && count < 3)
    {
        cout
        << count + 1
        << ". "
        << temp.top().second
        << " -> Rs."
        << temp.top().first
        << endl;

        temp.pop();
        count++;
    }
}

// =====================================================
// DYNAMIC PROGRAMMING DISCOUNT OPTIMIZER
// =====================================================

double CartMaxSystem::applyBestDiscount(
    vector<Coupon> coupons)
{
    int n = coupons.size();

    vector<double> dp(
        1 << n,
        currentCartTotal
    );

    double bestPrice =
    currentCartTotal;

    dp[0] = currentCartTotal;

    for(int mask = 0;
        mask < (1 << n);
        mask++)
    {
        bestPrice =
        min(
            bestPrice,
            dp[mask]
        );

        for(int i = 0;
            i < n;
            i++)
        {
            if(mask & (1 << i))
                continue;

            double newPrice =
            dp[mask];

            if(coupons[i]
               .isPercentage)
            {
                newPrice -=
                (
                    newPrice *
                    coupons[i].value
                    / 100.0
                );
            }
            else
            {
                newPrice -=
                coupons[i].value;
            }

            if(newPrice < 0)
                newPrice = 0;

            int nextMask =
            mask | (1 << i);

            dp[nextMask] =
            min(
                dp[nextMask],
                newPrice
            );
        }
    }

    return
    currentCartTotal
    -
    bestPrice;
}

// =====================================================
// ORDER HISTORY
// =====================================================

void CartMaxSystem::showOrderHistory()
{
    cout
    << "\n===== ORDER HISTORY =====\n";

    if(orderHistory.empty())
    {
        cout
        << "No Orders Yet\n";

        return;
    }

    for(int i = 0;
        i < orderHistory.size();
        i++)
    {
        cout
        << i + 1
        << ". "
        << orderHistory[i]
        << endl;
    }
}

// =====================================================
// MAIN FUNCTION
// =====================================================

int main()
{
    CartMaxSystem cms;

    // Coupons

    cms.addPromoCode("SAVE10");
    cms.addPromoCode("SAVE20");
    cms.addPromoCode("SUPER50");
    cms.addPromoCode("FLAT500");

    // Products

    cms.addProduct(
        "P1",
        "Laptop",
        50000,
        10
    );

    cms.addProduct(
        "P2",
        "Mouse",
        1000,
        50
    );

    cms.addProduct(
        "P3",
        "Keyboard",
        2500,
        25
    );

    cms.addProduct(
        "P4",
        "Headphones",
        3500,
        20
    );

    // Membership

    cms.addMember(
        "U101",
        "Premium"
    );

    cms.addMember(
        "U102",
        "Gold"
    );

    // Graph

    cms.addBundleRelationship(
        "Laptop",
        "Mouse"
    );

    cms.addBundleRelationship(
        "Laptop",
        "Keyboard"
    );

    cms.addBundleRelationship(
        "Keyboard",
        "Headphones"
    );

    int choice;

    do
    {
        cout
        << "\n\n========== CARTMAX ==========\n";

        cout << "1  View Products\n";
        cout << "2  Add To Cart\n";
        cout << "3  Remove From Cart\n";
        cout << "4  View Cart\n";
        cout << "5  Undo Last Change\n";
        cout << "6  Promo Suggestions\n";
        cout << "7  Membership Check\n";
        cout << "8  Product Recommendations\n";
        cout << "9  Add Customer To Checkout\n";
        cout << "10 View Checkout Queue\n";
        cout << "11 Process Checkout\n";
        cout << "12 Best Discount\n";
        cout << "13 Highest Value Cart\n";
        cout << "14 Top 3 Carts\n";
        cout << "15 Order History\n";
        cout << "0  Exit\n";

        cout << "\nEnter Choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                cms.displayProducts();
                break;
            }

            case 2:
            {
                string pid;
                int qty;

                cout
                << "Product ID: ";

                cin >> pid;

                cout
                << "Quantity: ";

                cin >> qty;

                cms.addToCart(
                    pid,
                    qty
                );

                break;
            }

            case 3:
            {
                string pid;

                cout
                << "Product ID: ";

                cin >> pid;

                cms.removeFromCart(
                    pid
                );

                break;
            }

            case 4:
            {
                cms.viewCart();
                break;
            }

            case 5:
            {
                cms.undoLastPriceChange();
                break;
            }

            case 6:
            {
                string prefix;

                cout
                << "Enter Prefix: ";

                cin >> prefix;

                vector<string> result =
                cms.getPromoSuggestions(
                    prefix
                );

                if(result.empty())
                {
                    cout
                    << "No Suggestions\n";
                }

                for(auto &c : result)
                {
                    cout
                    << c
                    << endl;
                }

                break;
            }

            case 7:
            {
                string id;

                cout
                << "Customer ID: ";

                cin >> id;

                cout
                << "Membership: "
                << cms.checkMembership(id)
                << endl;

                break;
            }

            case 8:
            {
                string product;

                cout
                << "Product Name: ";

                cin >> product;

                cms.getRecommendations(
                    product
                );

                break;
            }

            case 9:
            {
                string customer;

                cout
                << "Customer Name: ";

                cin >> customer;

                cms.addToCheckoutProcess(
                    customer
                );

                cms.logCartForMonitoring(
                    customer,
                    cms.getCartTotal()
                );

                break;
            }

            case 10:
            {
                cms.displayCheckoutQueue();
                break;
            }

            case 11:
            {
                cms.processNextInLine();
                break;
            }

            case 12:
            {
                vector<Coupon> coupons =
                {
                    {"SAVE10",true,10},
                    {"SAVE20",true,20},
                    {"FLAT500",false,500}
                };

                cout
                << "Best Discount: Rs."
                << cms.applyBestDiscount(
                    coupons
                )
                << endl;

                break;
            }

            case 13:
            {
                cms.getHighestValueCart();
                break;
            }

            case 14:
            {
                cms.displayTop3Carts();
                break;
            }

            case 15:
            {
                cms.showOrderHistory();
                break;
            }

            case 0:
            {
                cout
                << "\nThank You\n";
                break;
            }

            default:
            {
                cout
                << "\nInvalid Choice\n";
            }
        }

    } while(choice != 0);

    return 0;
} 