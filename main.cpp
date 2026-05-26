#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;

class Farmer;
class Player;

//CLASA ABSTRACTA PLANT
class Plant {
protected:
    string name;
    float growthDuration;
    int age;
    int buyPrice;
    int sellPrice;
public:
    Plant(const string&, float, int, int);
    //CONSTRUCTOR DE COPIERE
    Plant(const Plant &p);
    virtual ~Plant();  // destructor VIRTUAL (obligatoriu la mostenire)
    friend istream& operator>>(istream &is, Plant &obj);
    friend ostream& operator<<(ostream &os, const Plant &obj);
    Plant& operator=(const Plant &obj) {
        if(this!=&obj) {
            this->name=obj.name;
            this->growthDuration=obj.growthDuration;
            this->age=0;
            this->buyPrice=obj.buyPrice;
            this->sellPrice=obj.sellPrice;
        }
        return *this;
    }
    friend bool operator==(const Plant& pl1, const Plant& pl2);
    friend Farmer;
    friend Player;
    void grow() {age++;};
    void water() {age+=1;};
    bool isMature();
    bool isWilted();
    int getBuyPrice() {return buyPrice;};
    int getSellPrice() {return sellPrice;};
    string getName() const {return name;};
    virtual string getType() const = 0;
    virtual Plant* clone() const = 0;  // clone pattern pentru copiere polimorfa
};

bool operator==(const Plant& pl1, const Plant& pl2) {
    return pl1.name == pl2.name;
}

bool Plant::isMature() {
    if(age>=int(growthDuration) && age<=int(growthDuration*1.5))
        return true;
    return false;
}

bool Plant::isWilted() {
    if(age>=int(growthDuration*1.5))
        return true;
    return false;
}

Plant::Plant(const string& name, float growthDuration, int buyPrice, int sellPrice) {
    this->name = name;
    this->growthDuration = growthDuration;
    this->age = 0;
    this->buyPrice = buyPrice;
    this->sellPrice = sellPrice;
}

//constructorul de copiere
Plant::Plant(const Plant &obj) {
    this->name = obj.name;
    this->growthDuration = obj.growthDuration;
    this->age = 0;
    this->buyPrice = obj.buyPrice;
    this->sellPrice = obj.sellPrice;
}

istream& operator>>(istream &is, Plant &obj) {
    cout<<"Enter plant name: ";
    is >> ws;
    getline(is, obj.name);
    cout<<"Enter growth duration: ";
    is>>obj.growthDuration;
    cout<<"Enter buy price: ";
    is>>obj.buyPrice;
    cout<<"Enter sell price: ";
    is>>obj.sellPrice;
    obj.age=0;
    return is;
}

ostream& operator<<(ostream &os, const Plant &obj) {
    os<<"Type= "<<obj.getType()<<'\n';  // polimorfism: apeleaza getType-ul derivat
    os<<"Name= "<<obj.name<<'\n';
    os<<"Growth Duration= "<<obj.growthDuration<<'\n';
    os<<"Age= "<<obj.age<<'\n';
    os<<"Buy Price= "<<obj.buyPrice<<'\n';
    os<<"Sell Price= "<<obj.sellPrice<<'\n';
    return os;
}

Plant::~Plant() {
}


//SUB-CLASA VEGETABLE
class Vegetable : public Plant {
private:
    bool isRoot;
public:
    Vegetable(const string& name, float growthDuration, int buyPrice, int sellPrice, bool isRoot)
        : Plant(name, growthDuration, buyPrice, sellPrice), isRoot(isRoot) {}

    string getType() const override { return "Vegetable"; }
    Plant* clone() const override { return new Vegetable(*this); }
    bool getIsRoot() const { return isRoot; }
};

// a = mar.clone() | a = new Vegetable(mar)
// a = new int(5)

//SUB-CLASA FRUIT
class Fruit : public Plant {
private:
    string season;
public:
    Fruit(const string& name, float growthDuration, int buyPrice, int sellPrice, const string& season)
        : Plant(name, growthDuration, buyPrice, sellPrice), season(season) {}

    string getType() const override { return "Fruit"; }
    Plant* clone() const override { return new Fruit(*this); }
    string getSeason() const { return season; }
};


//CLASA PLAYER
class Player {
private:
    float balance;
    string name;
    string date;
    int noPlots;
    vector<Plant*> farm;  // VECTOR DE POINTERI — nullptr = plot gol
public:
    Player(const string&);
    ~Player();
    friend class Farmer;

    void buyPlot();
    void sellPlot(int index);
    int getNoPlots() {return noPlots;};
    void display();
    void agePlants() {
        for(auto p : farm)
            if(p != nullptr)
                p->grow();
    }

    Player& operator+=(float amount) {
        this->balance+=amount;
        return *this;
    }
};

Player::Player(const string& name) {
    this->balance = 50;
    this->name = name;
    this->date = "01/01/2026";
    this->noPlots = 1;
    farm.push_back(nullptr);  // plot gol
}

Player::~Player() {
    // sterg toate plantele detinute (delete pe nullptr e ok, nu face nimic)
    for(auto p : farm) delete p;
}

void Player::buyPlot() {
    balance-=15;
    noPlots+=1;
    farm.push_back(nullptr);  // nou plot, gol
};

void Player::sellPlot(int index) {
    if(index>=0 && index<noPlots) {
        balance+=10;
        noPlots-=1;
        delete farm[index];  // sterg planta din plot inainte sa-l elimin
        farm.erase(farm.begin()+index);
    }
};

void Player::display() {
        // Lambda existent pentru desenarea interiorului plotului
        auto box = [&](int i) -> string {
            if(i >= (int)farm.size()) return "| # # # |";
            if(farm[i] == nullptr) return "| - - - |";  // plot gol
            char c = farm[i]->getName()[0];             
            return string("| ") + c + " " + c + " " + c + " |";
        };

        // LAMBDA MODIFICAT: Afișează [READY], [WILTED] sau Age: X, păstrând fix 9 caractere
        auto ageLine = [&](int i) -> string {
            if (i >= (int)farm.size() || farm[i] == nullptr) {
                return "         "; // 9 spații pentru plot gol / neachiziționat
            }
            
            string statusStr;
            
            // Verificăm starea plantei folosind metodele tale
            if (farm[i]->isWilted()) {
                statusStr = "[WILTED]";
            } else if (farm[i]->isMature()) {
                statusStr = "[READY]";
            } else {
                statusStr = "Age: " + to_string(farm[i]->age);
            }
            
            // Forțăm string-ul să aibă FIX 9 caractere (umplem cu spații dacă e mai scurt)
            while (statusStr.length() < 9) {
                statusStr += " ";
            }
            
            // Dacă cumva textul e mai lung de 9 caractere (ex: la vârste de peste 2 cifre), 
            // îl tăiem la 9 ca să nu destabilizăm marginile ASCII
            return statusStr.substr(0, 9);
        };

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
|                                                                 |
)";
        // RÂNDUL 1 DE PLOTURI (Plot 1, 2, 3)
        cout << "|  " << ageLine(0) << "     " << ageLine(1) << "     " << ageLine(2) << "                       |\n";
        cout << "|  .-------.     .-------.     .-------.       [ OPTIONS ]        |\n";
        cout << "|  " << box(0) << "     " << box(1) << "     " << box(2) << "     (A) [ WATER ]        |\n";
        cout << "|  " << box(0) << "     " << box(1) << "     " << box(2) << "     (B) [ PLANT ]        |\n";
        cout << "|  " << box(0) << "     " << box(1) << "     " << box(2) << "     (C) [ HARVEST ]      |\n";
        cout << R"(|  '-------'     '-------'     '-------'     (D) [ BUY PLOT ]     |
|    Plot 1        Plot 2        Plot 3      (E) [ SELL PLOT ]    |
|                                            (F) [ CREATE PLANT ] |
|                                                                 |
)";
        // RÂNDUL 2 DE PLOTURI (Plot 4, 5, 6)
        cout << "|  " << ageLine(3) << "     " << ageLine(4) << "     " << ageLine(5) << "                          |\n";
        cout << "|  .-------.     .-------.     .-------.                          |\n";
        cout << "|  " << box(3) << "     " << box(4) << "     " << box(5) << "                          |\n";
        cout << "|  " << box(3) << "     " << box(4) << "     " << box(5) << "                          |\n";
        cout << "|  " << box(3) << "     " << box(4) << "     " << box(5) << "                          |\n";
        cout << R"(|  '-------'     '-------'     '-------'                          |
|    Plot 4        Plot 5        Plot 6                           |
|                                                                 |
)";
        // RÂNDUL 3 DE PLOTURI (Plot 7, 8, 9)
        cout << "|  " << ageLine(6) << "     " << ageLine(7) << "     " << ageLine(8) << "                          |\n";
        cout << "|  .-------.     .-------.     .-------.                          |\n";
        cout << "|  " << box(6) << "     " << box(7) << "     " << box(8) << "                          |\n";
        cout << "|  " << box(6) << "     " << box(7) << "     " << box(8) << "                          |\n";
        cout << "|  " << box(6) << "     " << box(7) << "     " << box(8) << "                          |\n";
        cout << R"(|  '-------'     '-------'     '-------'                          |
|    Plot 7        Plot 8        Plot 9                           |
|_________________________________________________________________|
| COMMAND: > _                                                    |
|_________________________________________________________________|
)";
}

class Farmer {
private:
    const int id;
    static int noFarmers;
    string name;
    bool isBusy;
    float salary;
    Player* employer;

public:
    Farmer(const string&, float);
    virtual ~Farmer();          // destructor virtual
    friend Plant;
    friend Player;
    void plant(Player &pl, Plant &p, int index);
    void harvest(int index);
    void pay(float amount);
    virtual void water(int index);   // VIRTUAL — suprascrisa de SeniorFarmer
    void setEmployer(Player* p);
};

int Farmer::noFarmers=0;

Farmer::Farmer(const string& name, float salary) : id(++noFarmers), employer(nullptr) {
    this->name = name;
    this->isBusy = false;
    this->salary = salary;
}

Farmer::~Farmer() {
}

void Farmer::plant(Player &pl, Plant &p, int index) {
    if(isBusy==true) {
        cout<<"Farmer is busy, return later.\n";
        return;
    }
    if(index < 0 || index >= (int)pl.farm.size()) {
        cout<<"Plot doesn't exist.\n";
        return;
    }
    if(pl.farm[index] != nullptr) {  // plot ocupat
        cout<<"Plot is not empty, choose another one.\n";
        return;
    }
    pl.farm[index] = p.clone();  // CLONE PATTERN — copie polimorfa
}

void Farmer::water(int index) {
    if(employer != nullptr
       && index >= 0
       && index < (int)employer->farm.size()
       && employer->farm[index] != nullptr)
        employer->farm[index]->water();
}
void Farmer::harvest(int index) {
    if (employer == nullptr) {
        cout << "Farmer has no employer to give the money to!\n";
        return;
    }
    if (index < 0 || index >= (int)employer->farm.size()) {
        cout << "Plot doesn't exist.\n";
        return;
    }
    if (employer->farm[index] == nullptr) {
        cout << "Nothing to harvest here.\n";
        return;
    }

    Plant* p = employer->farm[index];

    // Verificăm dacă planta este matură sau ofilită
    if (p->isWilted()) {
        cout << "Oh no! The plant '" << p->getName() << "' was wilted and couldn't be harvested.\n";
        delete p;
        employer->farm[index] = nullptr; // Eliberăm plotul
    } 
    else if (p->isMature()) {
        int profit = p->getSellPrice();
        *employer += profit; // Adăugăm banii în balanța jucătorului folosind operatorul +=
        cout << "Success! Harvested '" << p->getName() << "' for $" << profit << ".\n";
        
        delete p; // Ștergem planta din memorie
        employer->farm[index] = nullptr; // Resetăm plotul la gol
    } 
    else {
        cout << "The plant '" << p->getName() << "' is not mature yet! (Age: " << p->age << ")\n";
    }
}

void Farmer::setEmployer(Player* p) {
    this->employer=p;
}


//SUB-CLASA JUNIOR FARMER
class JuniorFarmer : public Farmer {
public:
    JuniorFarmer(const string& name) : Farmer(name, 50) {}  // salariu fix mic
    void water(int index) override {
        Farmer::water(index);
    }
};


//SUB-CLASA SENIOR FARMER
class SeniorFarmer : public Farmer {
private:
    int experience;
public:
    SeniorFarmer(const string& name, int experience)
        : Farmer(name, 150), experience(experience) {}  // salariu mai mare

    void water(int index) override {
        Farmer::water(index);      // uda plotul curent
        Farmer::water(index + 1);  // BONUS: si urmatorul plot!
    }

    int getExperience() const { return experience; }
};


int main() {
    // VECTOR DE POINTERI la Plant — putem amesteca Vegetables si Fruits
    vector<Plant*> availablePlants;
    availablePlants.push_back(new Vegetable("wheat", 3, 5, 10, false));
    availablePlants.push_back(new Vegetable("corn", 5, 10, 20, false));
    availablePlants.push_back(new Vegetable("potato", 4, 8, 15, true));   // radacinoasa
    availablePlants.push_back(new Vegetable("soy", 6, 12, 25, false));
    availablePlants.push_back(new Fruit("raspberry", 2, 4, 8, "summer"));
    availablePlants.push_back(new Fruit("tomato", 4, 7, 14, "summer"));

    Player player("Anto");
    Player player2("Luca");
    JuniorFarmer farmer("Boby");        // Junior — salariu $50
    farmer.setEmployer(&player);
    SeniorFarmer farmer2("Rob", 10);    // Senior cu 10 ani experienta
    Farmer farmer3("Alice", 110);       // farmer generic

    char cmd;
    int index;
    do {
        system("clear");
        player.display();
        cin>>cmd;
        switch(cmd) {
            case 'A': case 'a': {
                cout<<"Enter plot index: ";
                cin>>index;
                if(index>=1 && index<=player.getNoPlots())
                    farmer.water(index-1);
                else
                    cout<<"Plot doesn't exist.\n";
                break;
            }
            case 'B': case 'b': {
                string plantName;
                cout<<"Enter plot index: ";
                cin>>index;
                cout<<"Enter plant name: ";
                cin>>plantName;
                Plant* chosen = nullptr;
                for(int i = 0; i < (int)availablePlants.size(); i++) {
                    if(availablePlants[i]->getName() == plantName) {
                        chosen = availablePlants[i];  // deja pointer
                        break;
                    }
                }
                if(chosen != nullptr)
                    farmer.plant(player, *chosen, index-1);
                else
                    cout<<"Unknown plant.\n";
                break;
            }
            case 'C': case 'c': {
                cout << "Enter plot index to harvest: ";
                cin >> index;
                if (index >= 1 && index <= player.getNoPlots()) {
                    farmer.harvest(index - 1);
                } else {
                    cout << "Plot doesn't exist.\n";
                }
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            }
            case 'D': case 'd':
                player.buyPlot();
                break;
            case 'E': case 'e':
                // sell plot
                break;
            case 'F': case 'f': {
                int n;
                cout<<"Cate plante vrei sa creezi? ";
                cin>>n;
                for(int i = 0; i < n; i++) {
                    char type;
                    cout<<"Tip planta (v=vegetable, f=fruit): ";
                    cin>>type;

                    Plant* p = nullptr;
                    if(type == 'v' || type == 'V') {
                        int r;
                        cout<<"Este radacinoasa? (1=da, 0=nu): ";
                        cin>>r;
                        p = new Vegetable("temp", 1, 0, 0, r);
                    } else if(type == 'f' || type == 'F') {
                        string season;
                        cout<<"Sezon: ";
                        cin>>season;
                        p = new Fruit("temp", 1, 0, 0, season);
                    } else {
                        cout<<"Tip necunoscut, planta ignorata.\n";
                        continue;
                    }
                    cin >> *p;  // foloseste operator>> pentru numele si preturile
                    availablePlants.push_back(p);
                }
                cout<<"\n=== Plante disponibile ===\n";
                for(int i = 0; i < (int)availablePlants.size(); i++)
                    cout << *availablePlants[i];  // foloseste operator<< (cu polimorfism la getType)
                cout<<"\nApasa Enter ca sa continui...";
                cin.ignore();
                cin.get();
                break;
            }
            case 'Q': case 'q':
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Unknown command.\n";
                break;
        }
        player.agePlants();  // toate plantele cresc cu o unitate de varsta dupa fiecare comanda
    } while(cmd != 'Q' && cmd != 'q');

    // CURATARE memorie — sterg toate plantele alocate
    for(auto p : availablePlants) delete p;

    return 0;
}

//weak association (employer) DONE
//alt operator supraincarcat ca functie membra (+=) DONE
//alt operator supraincarcat ca functie non-membra (== la plante) DONE
//optiune ptr citire si afisarea a n obiecte (comanda F) DONE
//sa dau optiunea unui player sa-si construiasca propriile plante (comanda F) DONE
//Plant abstracta cu metode pur virtuale (getType, clone) DONE
//mostenire: Vegetable + Fruit din Plant, JuniorFarmer + SeniorFarmer din Farmer DONE
