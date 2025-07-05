#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

class IShippable {
public:
    virtual ~IShippable() = default;
    virtual string getName() = 0;
    virtual double getWeight() = 0;
};

class Product {
protected:
    string name;
    double price;
    int quantity;

public:
    Product(string n, double p, int q) : name(n), price(p), quantity(q) {}
    virtual ~Product() = default;

    string getProductName() { return name; }
    double getPrice() { return price; }
    int getQuantity() { return quantity; }

    void reduceQuantity(int q) { quantity -= q; }

    virtual bool isExpired() { return false; }
    virtual bool requiresShipping() { return false; }
    virtual double getShippingCost() { return 0; }
    virtual IShippable* getShippableInterface() { return nullptr; }
};

class ExpirableProduct : virtual public Product {
protected:
    int daysToExpire;

public:
    ExpirableProduct(string n, double p, int q, int days)
        : Product(n, p, q), daysToExpire(days) {}

    bool isExpired() override {
        return daysToExpire <= 0;
    }
};

class ShippableProduct : virtual public Product, public IShippable {
protected:
    double weight;

public:
    ShippableProduct(string n, double p, int q, double w)
        : Product(n, p, q), weight(w) {}

    bool requiresShipping() override { return true; }
    double getShippingCost() override { return 30; } // Fixed shipping cost

    IShippable* getShippableInterface() override {
        return this;
    }

    string getName() override { return name; }
    double getWeight() override { return weight; }
};

class Cheese : public ShippableProduct, public ExpirableProduct {
public:
    Cheese(double p, int q, double w, int days)
        : Product("Cheese", p, q),
          ShippableProduct("Cheese", p, q, w),
          ExpirableProduct("Cheese", p, q, days) {}

    bool isExpired() override {
        return ExpirableProduct::isExpired();
    }
};

class Biscuits : public ExpirableProduct, public ShippableProduct {
public:
    Biscuits(double p, int q, double w, int days)
        : Product("Biscuits", p, q),
          ExpirableProduct("Biscuits", p, q, days),
          ShippableProduct("Biscuits", p, q, w) {}

    bool isExpired() override {
        return ExpirableProduct::isExpired();
    }
};

class TV : public ShippableProduct {
public:
    TV(double p, int q, double w)
        : Product("TV", p, q),
          ShippableProduct("TV", p, q, w) {}
};

class ScratchCard : public Product {
public:
    ScratchCard(double p, int q)
        : Product("Mobile scratch cards", p, q) {}
};

class CartItem {
public:
    Product* product;
    int quantity;

    CartItem(Product* p, int q) : product(p), quantity(q) {}
};

class Cart {
    vector<CartItem> items;

public:
    void addProduct(Product* p, int quantity) {
        if (quantity > p->getQuantity()) {
            throw runtime_error(p->getProductName() + " - requested quantity exceeds available stock");
        }
        if (p->isExpired()) {
            throw runtime_error(p->getProductName() + " is expired");
        }
        items.push_back(CartItem(p, quantity));
    }

    vector<CartItem>& getItems() { return items; }
    bool isEmpty() { return items.empty(); }
};

class Customer {
    string name;
    double balance;

public:
    Customer(string n, double b) : name(n), balance(b) {}

    string getName() { return name; }
    double getBalance() { return balance; }

    void pay(double amount) {
        if (balance < amount) {
            throw runtime_error("Customer's balance is insufficient");
        }
        balance -= amount;
    }
};

class ShippingService {
public:
    static void ship(vector<CartItem>& items) {
        cout << "** Shipment notice **\n";
        double totalWeight = 0;

        for (auto& item : items) {
            if (item.product->requiresShipping()) {
                IShippable* shippableItem = item.product->getShippableInterface();
                if (shippableItem) {
                    double weightInGrams = shippableItem->getWeight() * 1000; // Convert kg to grams
                    totalWeight += shippableItem->getWeight() * item.quantity;
                    cout << item.quantity << "x " << shippableItem->getName()
                         << " " << (int)weightInGrams << "g\n";
                }
            }
        }

        cout << "Total package weight " << totalWeight << "kg\n";
    }
};

class Checkout {
public:
    static void process(Customer& customer, Cart& cart) {
        if (cart.isEmpty()) {
            throw runtime_error("Cart is empty");
        }

        double subtotal = 0;
        double shippingFees = 0;
        bool hasShippableItems = false;

        for (auto& item : cart.getItems()) {
            if (item.quantity > item.product->getQuantity()) {
                throw runtime_error(item.product->getProductName() + " is out of stock");
            }
            if (item.product->isExpired()) {
                throw runtime_error(item.product->getProductName() + " is expired");
            }

            subtotal += item.product->getPrice() * item.quantity;

            if (item.product->requiresShipping()) {
                hasShippableItems = true;
            }
        }

        if (hasShippableItems) {
            shippingFees = 30;
        }

        double total = subtotal + shippingFees;
        customer.pay(total);

        for (auto& item : cart.getItems()) {
            item.product->reduceQuantity(item.quantity);
        }

        if (hasShippableItems) {
            ShippingService::ship(cart.getItems());
        }

        cout << "** Checkout receipt **\n";
        for (auto& item : cart.getItems()) {
            cout << item.quantity << "x " << item.product->getProductName()
                 << " " << (int)(item.product->getPrice() * item.quantity) << "\n";
        }
        cout << "----------------------\n";
        cout << "Subtotal " << (int)subtotal << "\n";
        cout << "Shipping " << (int)shippingFees << "\n";
        cout << "Amount " << (int)total << "\n";
    }
};

int main() {
    Cheese* cheese = new Cheese(100, 10, 0.4, 5);
    Biscuits* biscuits = new Biscuits(150, 15, 0.7, 10);
    TV* tv = new TV(500, 5, 15);
    ScratchCard* scratchCard = new ScratchCard(10, 50);

    Customer customer1("Muhammed Saad", 1000);

    try {
        Cart cart1;
        cart1.addProduct(cheese, 2);
        cart1.addProduct(biscuits, 1);

        Checkout::process(customer1, cart1);
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    cout << "\n--- Test expired product ---\n";
    Cheese* expiredCheese = new Cheese(30, 5, 0.3, 0);
    Customer customer2("Hany Ahmed", 500);

    try {
        Cart cart2;
        cart2.addProduct(expiredCheese, 1);
        Checkout::process(customer2, cart2);
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    cout << "\n--- Test insufficient balance ---\n";
    Customer customer3("mohamed shaban", 100);

    try {
        Cart cart3;
        cart3.addProduct(tv, 1);
        Checkout::process(customer3, cart3);
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    cout << "\n--- Test empty cart ---\n";
    Customer customer4("mo_sh", 500);

    try {
        Cart cart4;
        Checkout::process(customer4, cart4);
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    cout << "\n--- Test out of stock ---\n";
    Customer customer5("not_mo_sh", 1000);

    try {
        Cart cart5;
        cart5.addProduct(cheese, 20);
        Checkout::process(customer5, cart5);
    } catch (exception& e) {
        cout << "Error: " << e.what() << "\n";
    }

    delete cheese;
    delete biscuits;
    delete tv;
    delete scratchCard;
    delete expiredCheese;

    return 0;
}
