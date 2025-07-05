#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Product {
    string name;
    double price, weight;
    int quantity, expiry;
    bool requiresShipping;

public:
    Product(string n, double p, int q, int exp = -1, double w = 0, bool ship = false)
        : name(n), price(p), quantity(q), expiry(exp), weight(w), requiresShipping(ship){}

    string getName() {
         return name;
         }
    double getPrice() {
        return price;
        }
    int getStock() {
        return quantity;
        }
    double getWeight() {
        return weight;
        }
    bool isExpired() {
        return expiry != -1 && expiry <= 0;
        }
    bool needsShipping() {
        return requiresShipping;
        }

    void reduceStock(int q) {
        quantity -= q;
        }
};

class Item {
public:
    Product* product;
    int quantity;

    Item(Product* p, int q) : product(p), quantity(q){}
};

class Cart {
    vector<Item> items;

public:
    void addItem(Product* p, int qty) {
        if (qty > p->getStock()) throw runtime_error(p->getName() + " out of stock");
        items.emplace_back(p, qty);
    }

    vector<Item>& getItems() {
        return items;
        }
    bool isEmpty() {
        return items.empty();
        }
};

class Customer {
    string name;
    double balance;

public:
    Customer(string n, double b) : name(n), balance(b) {}

    void pay(double amount) {
        if (balance < amount) throw runtime_error("Not enough balance");
        balance -= amount;
    }

    double getBalance() {
        return balance;
        }
};

class Shipping {
public:
    static void shipItems(const vector<pair<string, double>>& packages) {
        cout << "** Shipment Notice **\n";
        double totalWeight = 0;
        for (auto& [name, weight] : packages) {
            cout << "1x " << name << endl;
            totalWeight += weight;
        }
        cout << "Total package weight: " << totalWeight << "kg\n";
    }
};

class Checkout {
public:
    static void process(Customer& customer, Cart& cart) {
        if (cart.isEmpty()) throw runtime_error("Cart is empty");

        double subtotal = 0, shippingCost = 0;
        vector<pair<string, double>> shippingList;

        for (auto& item : cart.getItems()) {
            Product* p = item.product;
            int qty = item.quantity;

            if (p->isExpired()) throw runtime_error(p->getName() + " is expired");

            subtotal += qty * p->getPrice();

            if (p->needsShipping()) {
                for (int i = 0; i < qty; ++i)
                    shippingList.emplace_back(p->getName(), p->getWeight());
                shippingCost += 10 * qty;
            }
        }

        double total = subtotal + shippingCost;
        customer.pay(total);

        for (auto& item : cart.getItems())
            item.product->reduceStock(item.quantity);

        if (!shippingList.empty())
            Shipping::shipItems(shippingList);

        cout << "\n** Receipt **\n";
        for (auto& item : cart.getItems())
            cout << item.quantity << "x " << item.product->getName()
                 << " = " << item.quantity * item.product->getPrice() << "\n";
        cout << "Subtotal: " << subtotal << "\n";
        cout << "Shipping: " << shippingCost << "\n";
        cout << "Total: " << total << "\n";
        cout << "Remaining Balance: " << customer.getBalance() << "\n";
    }
};

int main() {
    Product* cheese = new Product("Cheese", 100, 5, 3, 0.2, true);
    Product* biscuits = new Product("Biscuits", 150, 2, 2, 0.7, true);
    Product* tv = new Product("TV", 300, 3, -1, 10, true);
    Product* card = new Product("Scratch Card", 50, 10);

    try {
        Customer c1("Muhamme_Saad", 1000);
        Cart cart1;
        cart1.addItem(cheese, 2);
        cart1.addItem(biscuits, 1);
        cart1.addItem(card, 1);
        Checkout::process(c1, cart1);
    } catch (exception& e) {
        cout << "[Error] " << e.what() << endl;
    }

    cout<<endl;

    try {
        Customer c2("MO_SH", 200);
        Cart cart2;
        cart2.addItem(tv, 2);
        Checkout::process(c2, cart2);
    } catch (exception& e) {
        cout << "[Error] " << e.what() << endl;
    }

        cout<<endl;


       try {
        Product* oldMilk = new Product("Old Milk", 80, 2, 0, 0.3, true);
        Customer c3("Hany_Ahmed", 500);
        Cart cart3;
        cart3.addItem(oldMilk, 1);
        Checkout::process(c3, cart3);
        delete oldMilk;
    } catch (exception& e) {
        cout << "[Error] " << e.what() << endl;
    }

        cout<<endl;


    try {
        Customer c4("EmptyCart", 100);
        Cart cart4;
        Checkout::process(c4, cart4);
    } catch (exception& e) {
        cout << "[Error] " << e.what() << endl;
    }

        cout<<endl;

    try {
        Customer c5("OutStock", 500);
        Cart cart5;
        cart5.addItem(biscuits, 100);
        Checkout::process(c5, cart5);
    } catch (exception& e) {
        cout << "[Error] " << e.what() << endl;
    }

    delete cheese;
    delete biscuits;
    delete tv;
    delete card;
    return 0;
}
