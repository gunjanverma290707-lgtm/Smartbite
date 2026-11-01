
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

// BASE CLASS
class MenuItem {
protected:
    int id;
    string name, category;
    double price;
    bool veg;

public:
    MenuItem(int i, string n, string c, double p, bool v)
        : id(i), name(n), category(c), price(p), veg(v) {}

    virtual string getCategory() const { return category; }
    virtual double packaging() const { return 5; }

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    bool isVeg() const { return veg; }

    double finalPrice() const {
        return price + packaging();
    }

    virtual void display() const {
        cout << id << ". " << name << " Rs."
             << price << (veg ? " [VEG]" : " [NON-VEG]") << endl;
    }

    virtual ~MenuItem() {}

    friend ostream& operator<<(ostream& out, MenuItem& m) {
        return out << m.name << " Rs." << m.price;
    }
};

// INHERITANCE
class Starter : public MenuItem {
public:
    using MenuItem::MenuItem;
    double packaging() const override { return 5; }
};

class MainCourse : public MenuItem {
public:
    using MenuItem::MenuItem;
    double packaging() const override { return 10; }
};

class Beverage : public MenuItem {
public:
    using MenuItem::MenuItem;
    double packaging() const override { return 3; }
};

class Dessert : public MenuItem {
public:
    using MenuItem::MenuItem;
    double packaging() const override { return 5; }
};

// CUSTOMER CLASS
class Customer {
    string name, phone;

public:
    Customer(string n, string p) : name(n), phone(p) {}

    string getName() const { return name; }
};

// MENU CLASS
class Menu {
    vector<MenuItem*> items;

public:
    Menu() {
        items.push_back(new Starter(1,"Paneer Tikka","Starter",180,1));
        items.push_back(new Starter(2,"French Fries","Starter",90,1));
        items.push_back(new Starter(3,"Chicken Wings","Starter",220,0));
        items.push_back(new MainCourse(4,"Veg Biryani","Main",200,1));
        items.push_back(new MainCourse(5,"Chicken Biryani","Main",260,0));
        items.push_back(new MainCourse(6,"Veg Burger","Main",110,1));
        items.push_back(new Beverage(7,"Cold Coffee","Beverage",90,1));
        items.push_back(new Beverage(8,"Mango Lassi","Beverage",80,1));
        items.push_back(new Dessert(9,"Gulab Jamun","Dessert",70,1));
        items.push_back(new Dessert(10,"Brownie","Dessert",130,1));
    }

    ~Menu() {
        for (auto p : items) delete p;
    }

    MenuItem* findItem(int id) {
        for (auto p : items)
            if (p->getId() == id) return p;
        return nullptr;
    }

    void displayMenu() {
        for (auto p : items) p->display();
    }

    vector<MenuItem*> getItems() {
        return items;
    }

    void search(string keyword) {
        for (auto p : items)
            if (p->getName().find(keyword) != string::npos)
                p->display();
    }
};

// ORDER CLASS
class Order {
    string customer;
    vector<pair<MenuItem*, int>> cart;
    double budget;

public:
    Order(string n, double b = 0) : customer(n), budget(b) {}

    void addItem(MenuItem* item, int qty = 1) {
        if (!item || qty <= 0) return;

        double total = 0;
        for (auto p : cart)
            total += p.first->finalPrice() * p.second;

        double newTotal = total + item->finalPrice() * qty;

        if (budget > 0 && newTotal * 1.05 > budget) {
            cout << "Budget exceeded!\n";
            return;
        }

        for (auto& p : cart) {
            if (p.first == item) {
                p.second += qty;
                return;
            }
        }

        cart.push_back({item, qty});
        cout << "Item added!\n";
    }

    double total() {
        double sum = 0;
        for (auto p : cart)
            sum += p.first->finalPrice() * p.second;
        return sum * 1.05;
    }

    void bill() {
        cout << "\n------ SMARTBITE BILL ------\n";
        cout << "Customer: " << customer << endl;

        for (auto p : cart)
            cout << p.first->getName()
                 << " x " << p.second << endl;

        cout << "GST included (5%)\n";
        cout << "Total: Rs." << fixed << setprecision(2)
             << total() << endl;

        ofstream file("orders.txt", ios::app);
        file << customer << " | Rs." << total() << endl;
    }
};

// SMART COMBO GENERATOR
void comboGenerator(Menu& menu, double budget, bool veg) {
    cout << "\nSuggested food items:\n";

    for (auto p : menu.getItems()) {
        if ((!veg || p->isVeg()) &&
            p->finalPrice() * 1.05 <= budget)
            p->display();
    }
}

// MAIN FUNCTION
int main() {
    Menu menu;

    string name, phone;
    cout << "Enter your name: ";
    getline(cin, name);

    cout << "Enter phone number: ";
    getline(cin, phone);

    Customer customer(name, phone);

    int choice;

    do {
        cout << "\n===== SMARTBITE =====\n";
        cout << "1. View Menu\n";
        cout << "2. Search Food\n";
        cout << "3. Normal Order\n";
        cout << "4. Budget Ordering\n";
        cout << "5. Smart Combo Generator\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
            menu.displayMenu();

        else if (choice == 2) {
            string key;
            cout << "Enter food name: ";
            cin >> ws;
            getline(cin, key);
            menu.search(key);
        }

        else if (choice >= 3 && choice <= 5) {
            double budget = 0;

            if (choice != 3) {
                cout << "Enter budget: ";
                cin >> budget;
            }

            if (choice == 5) {
                int v;
                cout << "1. Veg only  2. All food: ";
                cin >> v;
                comboGenerator(menu, budget, v == 1);
            }
            else {
                Order order(customer.getName(), budget);

                int id, qty;

                while (true) {
                    menu.displayMenu();
                    cout << "Enter item ID (0 to finish): ";
                    cin >> id;

                    if (id == 0) break;

                    MenuItem* item = menu.findItem(id);

                    if (!item) {
                        cout << "Invalid ID!\n";
                        continue;
                    }

                    cout << "Enter quantity: ";
                    cin >> qty;
                    order.addItem(item, qty);
                }

                order.bill();
            }
        }

    } while (choice != 6);

    cout << "Thank you for choosing SmartBite!\n";
    return 0;
}
