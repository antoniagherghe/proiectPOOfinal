#include <iostream>
#include <string>
#include <vector>    //structura1
#include <list>       //structura2
#include <unistd.h>

using namespace std;


enum class Season { SPRING, SUMMER, AUTUMN, WINTER }; 

class Farmer;
class Player;

class InsufficientFundsException : public exception { 
public:
    const char* what() const noexcept override {
        return "Eroare: Nu ai suficienti bani in balanta pentru a plati fermierul sau actiunea!";
    }
};

class InvalidPlotException : public exception { 
public:
    const char* what() const noexcept override {
        return "Eroare: Plotul selectat este invalid sau nu a fost cumparat inca!";
    }
};

class Plant {
protected:
    string name;
    float growthDuration;
    int age;
    int buyPrice;
    int sellPrice;
public:
    Plant(const string&, float, int, int);
    Plant(const Plant &p);
    virtual ~Plant();  
    
    friend istream& operator>>(istream &is, Plant &obj); 
    friend ostream& operator<<(ostream &os, const Plant &obj); 
    
    Plant& operator=(const Plant &obj) { 
        if(this != &obj) {
            this->name = obj.name;
            this->growthDuration = obj.growthDuration;
            this->age = 0;
            this->buyPrice = obj.buyPrice;
            this->sellPrice = obj.sellPrice;
        }
        return *this;
    }
    
    friend bool operator==(const Plant& pl1, const Plant& pl2); 
    friend class Farmer;
    friend class Player;
    
    void grow() { age++; };
    void water() { age += 1; };
    bool isMature();
    bool isWilted();
    int getBuyPrice() const { return buyPrice; };
    int getSellPrice() const { return sellPrice; };
    string getName() const { return name; };
    
    virtual string getType() const = 0; 
    virtual Plant* clone() const = 0;  
};

bool operator==(const Plant& pl1, const Plant& pl2) {
    return pl1.name == pl2.name;
}

bool Plant::isMature() {
    return (age >= int(growthDuration) && age <= int(growthDuration * 1.5));
}

bool Plant::isWilted() {
    return (age >= int(growthDuration * 1.5));
}

Plant::Plant(const string& name, float growthDuration, int buyPrice, int sellPrice) { 
    this->name = name;
    this->growthDuration = growthDuration;
    this->age = 0;
    this->buyPrice = buyPrice;
    this->sellPrice = sellPrice;
}

Plant::Plant(const Plant &obj) { 
    this->name = obj.name;
    this->growthDuration = obj.growthDuration;
    this->age = 0;
    this->buyPrice = obj.buyPrice;
    this->sellPrice = obj.sellPrice;
}

istream& operator>>(istream &is, Plant &obj) { 
    cout << "Enter plant name: ";
    is >> ws;
    getline(is, obj.name);
    cout << "Enter growth duration: ";
    is >> obj.growthDuration;
    cout << "Enter buy price: ";
    is >> obj.buyPrice;
    cout << "Enter sell price: ";
    is >> obj.sellPrice;
    obj.age = 0;
    return is;
}

ostream& operator<<(ostream &os, const Plant &obj) { 
    os << "Type= " << obj.getType() << '\n';  
    os << "Name= " << obj.name << '\n';
    os << "Growth Duration= " << obj.growthDuration << '\n';
    os << "Age= " << obj.age << '\n';
    os << "Buy Price= " << obj.buyPrice << '\n';
    os << "Sell Price= " << obj.sellPrice << '\n';
    return os;
}

Plant::~Plant() {} 



class Vegetable : public Plant { 
private:
    bool isRoot;
public:
    Vegetable(const string& name, float growthDuration, int buyPrice, int sellPrice, bool isRoot)
        : Plant(name, growthDuration, buyPrice, sellPrice), isRoot(isRoot) {}

    Vegetable(const Vegetable& other) : Plant(other), isRoot(other.isRoot) {}

    Vegetable& operator=(const Vegetable& other) {
        if (this != &other) {
            Plant::operator=(other);
            this->isRoot = other.isRoot;
        }
        return *this;
    }

    string getType() const override { return "Vegetable"; }
    Plant* clone() const override { return new Vegetable(*this); }
    bool getIsRoot() const { return isRoot; }
};


class Fruit : public Plant { 
private:
    Season preferentialSeason; 
public:
    Fruit(const string& name, float growthDuration, int buyPrice, int sellPrice, Season season)
        : Plant(name, growthDuration, buyPrice, sellPrice), preferentialSeason(season) {} 

    string getType() const override { return "Fruit"; }
    Plant* clone() const override { return new Fruit(*this); }
    string getSeasonStr() const {
        if(preferentialSeason == Season::SUMMER) return "Summer";
        return "Other Season";
    }
};

template <typename T>
void displayPlantFinancialReport(const T& plant) {
    int profitCurat = plant.getSellPrice() - plant.getBuyPrice();

    cout << " Produs: " << plant.getName() << " [" << plant.getType() << "]\n";
    cout << " Cost seminte: $" << plant.getBuyPrice() << "\n";
    cout << " Recoltare: $" << plant.getSellPrice() << "\n";
    cout << " Diferenta: $" << profitCurat << "\n";
}



class Plot { 
private:
    Plant* plantedCrop; 
    bool isUnlocked; 
public:
    Plot() : plantedCrop(nullptr), isUnlocked(false) {} 
    ~Plot() { delete plantedCrop; }
    
    void setCrop(Plant* p) {
        delete plantedCrop;
        plantedCrop = p;
    }
    Plant* getCrop() { return plantedCrop; }
    void clear() { plantedCrop = nullptr; }
    
    
    bool checkUnlocked() const { return isUnlocked; }
    void unlock() { isUnlocked = true; }
    void lock() { isUnlocked = false; }
};



class Player {
private:
    float balance;
    string name;
    string date;
    int noPlots;
    
    vector<Plot> farm;  
public:
    Player(const string&);
    ~Player();
    friend class Farmer;

    void buyPlot();
    void sellPlot(int index);
    int getNoPlots() { return noPlots; };
    float getBalance() const { return balance; }
    void display();
    
    void agePlants() {
        for(size_t i = 0; i < farm.size(); i++)
            if(farm[i].getCrop() != nullptr)
                farm[i].getCrop()->grow();
    }

    Player& operator+=(float amount) { 
        this->balance += amount;
        return *this;
    }
    Player& operator-=(float amount) { 
        this->balance -= amount;
        return *this;
    }
};

Player::Player(const string& name) {
    this->balance = 500; 
    this->name = name;
    this->date = "01/01/2026";
    this->noPlots = 1;
    
    
    for(int i = 0; i < 6; i++) {
        farm.push_back(Plot());
    }
    farm[0].unlock(); 
}

Player::~Player() {}

void Player::buyPlot() {
    if (balance < 15) {
        throw InsufficientFundsException(); 
    }
    if (noPlots >= 6) {
        cout << "\nAi atins numarul maxim de ploturi (6)!\n";
        return;
    }
    
   
    farm[noPlots].unlock(); 
    balance -= 15;
    noPlots += 1;
}

void Player::sellPlot(int index) {
    if(index < 0 || index >= noPlots || index == 0) { 
        
        throw InvalidPlotException(); 
    }
    
    
    farm[index].setCrop(nullptr);
    farm[index].lock();
    
    balance += 10;
    noPlots -= 1;
}



class Farmer {
private:
    const int id;
    static int noFarmers; 
protected:
    string name;
    bool isBusy;
    float salary;
    Player* employer; 

public:
    Farmer(const string&, float);
    virtual ~Farmer();          
    friend class Plant;
    friend class Player;
    
    string getName() const { return name; }
    float getSalary() const { return salary; }
    
    void plant(Player &pl, Plant &p, int index);
    void harvest(int index);
    virtual void water(int index);   
    void setEmployer(Player* p);
    
    static void printFarmerCountInfo() {
        cout << "Sunt "  << noFarmers << " fermieri in baza de date.\n";
    }
};  

int Farmer::noFarmers = 0; 

Farmer::Farmer(const string& name, float salary) : id(++noFarmers), employer(nullptr) { 
    this->name = name;
    this->isBusy = false;
    this->salary = salary;
}

Farmer::~Farmer() {}

void Farmer::plant(Player &pl, Plant &p, int index) {
    if (pl.getBalance() < salary) throw InsufficientFundsException();
    if(index < 0 || index >= (int)pl.farm.size()) throw InvalidPlotException();
    
    if(pl.farm[index].getCrop() != nullptr) {  
        cout << "Plot-ul nu este gol!\n";
        return;
    }
    pl -= salary; 
    pl.farm[index].setCrop(p.clone());  
    cout << name << " a plantat cu succes " << p.getName() << " pe plotul " << (index + 1) << ".\n";
}

void Farmer::water(int index) {
    if (employer == nullptr) return;
    if (employer->getBalance() < salary) throw InsufficientFundsException();
    if (index < 0 || index >= (int)employer->farm.size()) throw InvalidPlotException();

    employer->operator-=(salary); 
    if (employer->farm[index].getCrop() != nullptr) {
        employer->farm[index].getCrop()->water();
        cout << name << " a udat plotul " << (index + 1) << ".\n";
    } else {
        cout << name << " a udat un plot gol .\n";
    }
}

void Farmer::harvest(int index) {
    if (employer == nullptr) return;
    if (employer->getBalance() < salary) throw InsufficientFundsException();
    if (index < 0 || index >= (int)employer->farm.size()) throw InvalidPlotException();
    
    if (employer->farm[index].getCrop() == nullptr) {
        cout << "Nimic de recoltat aici.\n";
        return;
    }

    employer->operator-=(salary); 
    Plant* p = employer->farm[index].getCrop();

    if (p->isWilted()) {
        cout << "Oh nu! Planta '" << p->getName() << "' a murit.\n";
        employer->farm[index].setCrop(nullptr); 
    } 
    else if (p->isMature()) {
        int profit = p->getSellPrice();
        *employer += profit; 
        cout << "Success! " << name << " a recoltat '" << p->getName() << "' pentru $" << profit << ".\n";
        employer->farm[index].setCrop(nullptr); 
    } 
    else {
        cout << "Planta '" << p->getName() << "' nu este inca matura! (Age: " << p->age << ")\n";
    }
}

void Farmer::setEmployer(Player* p) { this->employer = p; }

class JuniorFarmer : public Farmer { 
public:
    JuniorFarmer(const string& name) : Farmer(name, 10) {} 
    void water(int index) override { Farmer::water(index); }
};

class SeniorFarmer : public Farmer { 
private:
    int experience;
public:
    SeniorFarmer(const string& name, int experience) : Farmer(name, 25), experience(experience) {} // Cost mai mare

    void water(int index) override {
        
        Farmer::water(index);      
        
        
        if (employer != nullptr && (index + 1) < employer->getNoPlots()) {
            
            Farmer::water(index + 1);
            cout << "[BONUS SENIOR] " << name << " a udat automat si plotul " << (index + 2) << "!\n";
        }
    }
    int getExperience() const { return experience; }
};

class GameStatManager { 
private:
    int totalHarvestsSuccessfully;
    GameStatManager() : totalHarvestsSuccessfully(0) {} 
    static GameStatManager* instance;
public:
    static GameStatManager* getInstance() { 
        if (!instance) instance = new GameStatManager();
        return instance;
    }
    void incrementHarvests() { totalHarvestsSuccessfully++; }
    void printStats() {
        cout << "+STATISTICI+ \nRecolte reusite: " 
             << totalHarvestsSuccessfully;
    }
};
GameStatManager* GameStatManager::instance = nullptr;

template <typename T>
bool isValueAffordable(T balance, T price) { 
    return balance >= price;
};


void Player::display() {
    auto box = [&](int i) -> string {
        if(!farm[i].checkUnlocked()) return "| # # # |"; // Dacă e blocat, afișează #
        if(farm[i].getCrop() == nullptr) return "| - - - |";  // Plot deblocat, dar gol
        char c = farm[i].getCrop()->getName()[0];             
        return string("| ") + c + " " + c + " " + c + " |";
    };
    auto ageLine = [&](int i) -> string {
        if (!farm[i].checkUnlocked() || farm[i].getCrop() == nullptr) return "         "; 
        string statusStr;
        Plant* p = farm[i].getCrop();
        if (p->isWilted()) statusStr = "[WILTED]";
        else if (p->isMature()) statusStr = "[READY]";
        else statusStr = "Age: " + to_string(p->age);
        
        while (statusStr.length() < 9) statusStr += " ";
        return statusStr.substr(0, 9);
    };
    string a0 = ageLine(0), a1 = ageLine(1), a2 = ageLine(2);
    string b0 = box(0),      b1 = box(1),      b2 = box(2);
    
    string a3 = ageLine(3), a4 = ageLine(4), a5 = ageLine(5);
    string b3 = box(3),      b4 = box(4),      b5 = box(5);

    cout << " _________________________________________________________________ \n";
    cout << "| [ " << date << " ]                             [ BAL: $" << balance << " ]     |\n";
    cout << R"(|_________________________________________________________________|
|                                                                 |
|      (  )                                                       |
|       ||           _   _                                        |
|      _||_         ( )_( )        * .   * |
|     /____\       (_   _)             .                          |
|    |  []  |        (_)          .                               |
|____|__L___|_____________________________________________________|
|                                                                 |)";
    cout << "\n|    " << a0 << "     " << a1 << "     " << a2 << "                            |\n";
    cout << "|  .-------.     .-------.     .-------.                          |\n";
    cout << "|  " << b0 << "     " << b1 << "     " << b2 << "                          |\n";
    cout << "|  " << b0 << "     " << b1 << "     " << b2 << "                          |\n";
    cout << "|  " << b0 << "     " << b1 << "     " << b2 << "                          |\n";
    cout << "|  '-------'     '-------'     '-------'                          |\n";
    cout << "|    Plot 1        Plot 2        Plot 3                           |\n";
    cout << "|                                                                 |\n";
    cout << "|      " << a3 << "      " << a4 << "     " << a5 << "                            |\n";
    cout << "|  .-------.     .-------.     .-------.                          |\n";
    cout << "|  " << b3 << "     " << b4 << "     " << b5 << "                          |\n";
    cout << "|  " << b3 << "     " << b4 << "     " << b5 << "                          |\n";
    cout << "|  " << b3 << "     " << b4 << "     " << b5 << "                          |\n";
    cout << "|  '-------'     '-------'     '-------'                          |\n";
    cout << "|    Plot 4        Plot 5        Plot 6                           |\n";
    cout << R"(|_________________________________________________________________|
| [ OPTIONS ]                                                     |
|  (A) [ WATER ]          (D) [ BUY PLOT ]      (G) [ STAFF INFO ]|
|  (B) [ PLANT ]          (E) [ SELL PLOT ]                       |
|  (C) [ HARVEST ]        (F) [ CREATE PLANT ]  (Q) [ QUIT GAME ] |
|_________________________________________________________________|
 COMMAND: > )";
}

Farmer* chooseFarmer(const list<Farmer*>& staff) {
    cout << "\nAlege fermierul:\n";
    int i = 1;
    vector<Farmer*> tempVec;
    for (Farmer* f : staff) {
        cout << i << ". " << f->getName() << " (Cost/actiune: $" << f->getSalary() << ")\n";
        tempVec.push_back(f);
        i++;
    }
    cout << "Optiune: ";
    int opt; cin >> opt;
    if (opt >= 1 && opt <= (int)tempVec.size()) {
        return tempVec[opt - 1];
    }
    return tempVec[0]; 
}


// MAIN FUNCTION
int main() {
    vector<Plant*> availablePlants;
    list<Farmer*> staffList; 

    availablePlants.push_back(new Vegetable("wheat", 3, 5, 10, false));
    availablePlants.push_back(new Vegetable("potato", 4, 8, 15, true));
    availablePlants.push_back(new Fruit("raspberry", 2, 4, 8, Season::SUMMER)); 

    Player player("Anto");
    
    JuniorFarmer* jFarmer = new JuniorFarmer("Boby");
    jFarmer->setEmployer(&player);
    staffList.push_back(jFarmer);

    SeniorFarmer* sFarmer = new SeniorFarmer("Rob", 10);
    sFarmer->setEmployer(&player);
    staffList.push_back(sFarmer);

    Farmer::printFarmerCountInfo(); 
    sleep(1);

    char cmd;
    int index;
    do {
        system("clear");
        player.display();
        cin >> cmd;
        
        try {
            switch(cmd) {
                case 'A': case 'a': {
                    Farmer* worker = chooseFarmer(staffList);
                    cout << "Enter plot index to water: "; cin >> index;
                    if(index >= 1 && index <= player.getNoPlots()) {
                        worker->water(index - 1);
                    } else throw InvalidPlotException(); 
                    cout << "\nApasa Enter..."; cin.ignore(); cin.get();
                    break;
                }
                case 'B': case 'b': {
                    Farmer* worker = chooseFarmer(staffList);
                    string plantName;
                    cout << "Enter plot index: "; cin >> index;
                    cout << "Enter plant name (" ;
                    for(auto p : availablePlants) cout << p->getName() << " ";
                    cout << "): "; cin >> plantName;
                    
                    Plant* chosen = nullptr;
                    for(auto p : availablePlants) {
                        if(p->getName() == plantName) { chosen = p; break; }
                    }
                    
                    if(chosen != nullptr) {
                        if(!isValueAffordable(player.getBalance(), worker->getSalary() + chosen->getBuyPrice())) {
                            throw InsufficientFundsException(); 
                        }
                        worker->plant(player, *chosen, index - 1);
                    } else cout << "Planta necunoscuta.\n";
                    cout << "\nApasa Enter..."; cin.ignore(); cin.get();
                    break;
                }
                case 'C': case 'c': {
                    Farmer* worker = chooseFarmer(staffList);
                    cout << "Introdu indexul plotului de recoltat: "; cin >> index;
                    worker->harvest(index - 1);
                    GameStatManager::getInstance()->incrementHarvests(); 
                    cout << "\nPress Enter..."; cin.ignore(); cin.get();
                    break;
                }
                case 'D': case 'd': {
                    player.buyPlot(); 
                    cout << "\nApasa Enter pentru a continua...";
                    cin.ignore();
                    cin.get();
                    break;
                }
                case 'E': case 'e': {
                    cout << "Introdu indexul plotului de vandut: "; cin >> index;
                    player.sellPlot(index - 1); 
                    cout << "\nApasa Enter pentru a continua...";
                    cin.ignore();
                    cin.get();
                    break;
                }
                case 'G': case 'g': {
                    cout << "Staff\n"; 
                    for (Farmer* f : staffList) {
                        cout << "Fermierul: " << f->getName() << "\n";
                        
                        SeniorFarmer* s = dynamic_cast<SeniorFarmer*>(f);
                        if (s != nullptr) { 
                            cout << s->getName() << " este Senior si are experienta: " 
                                 << s->getExperience() << " ani.\n"; 
                        }
                    }
                    GameStatManager::getInstance()->printStats(); 
                    cout << "\nApasa Enter..."; cin.ignore(); cin.get();
                    break;
                }
                case 'F': case 'f': {
                    int n; cout << "Cate plante creezi? "; cin >> n;
                    for(int i = 0; i < n; i++) {
                        char type; cout << "Tip (v/f): "; cin >> type;
                        Plant* p = nullptr;
                        if(type == 'v' || type == 'V') {
                            p = new Vegetable("temp", 1, 0, 0, false);
                        } else {
                            p = new Fruit("temp", 1, 0, 0, Season::SPRING); 
                        }
                        
                        cin >> *p;
                        availablePlants.push_back(p);
                        displayPlantFinancialReport(*p); 
                    }
                    cout << "\nApasa Enter ca sa continui...";
                    cin.ignore(); cin.get();
                    break;
                }
            }
        } 
        catch (const InsufficientFundsException& e) { 
            cout << "\n" << e.what() << "\nApasa Enter ca sa continui...";
            cin.ignore(); cin.get();
        } 
        catch (const InvalidPlotException& e) { 
            cout << "\n" << e.what() << "\nApasa Enter ca sa continui...";
            cin.ignore(); cin.get();
        } 
    } while(cmd != 'Q' && cmd != 'q');
    for(auto p : availablePlants) delete p;
    for(auto f : staffList) delete f;

    return 0;
}