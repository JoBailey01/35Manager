#include "35Manager.h"

//Ensure that a die size is valid. Default to d8 if it is not.
int checkDieSize(int input){
    switch (input) {
        case 2:
        case 3:
        case 4:
        case 6:
        case 8:
        case 10:
        case 12:
        case 20:
            return input;
        default:
            return 8;
    }
}

//Ensure that BAB progression is valid. Default to 1/2 (2/4) if it is not.
int checkBAB(int input){
    return input >= 2 && input <= 4 ? input : 2;
}

//Get BAB progression as a name with a fraction
std::string getBABProgression(int BAB){
    //Full BAB
    if(BAB == 4){
        return std::string("1 HD");
    //Even-numbered BAB fraction (2)
    } else if (BAB % 2 == 0){
        return std::to_string(BAB/2) + "/2 HD";
    //Odd-numbered BAB
    } else {
        return std::to_string(BAB) + "/4 HD";
    }
};


//Get the name of an ability score based on its abbreviation from the ability enum
std::string abilityName(int abilityNum){
    switch (abilityNum) {
        case 0: return std::string("Strength");
        case 1: return std::string("Dexterity");
        case 2: return std::string("Constitution");
        case 3: return std::string("Intelligence");
        case 4: return std::string("Wisdom");
        case 5: return std::string("Charisma");
        default: return std::string("[Invalid Ability]");
    }
}

//Get the abbreviated name of an ability score based on its abbreviation/number from the ability enum
std::string abilityAbbr(int abilityNum){
    switch (abilityNum) {
        case 0: return std::string("STR");
        case 1: return std::string("DEX");
        case 2: return std::string("CON");
        case 3: return std::string("INT");
        case 4: return std::string("WIS");
        case 5: return std::string("CHA");
        default: return std::string("[Invalid]");
    }
}


//Define SavingThrows constructor
SavingThrows::SavingThrows(int fort, int ref, int will){
    this-> fort = fort;
    this-> ref = ref;
    this-> will = will;
}

//Define blank SavingThrows constructor
SavingThrows::SavingThrows(){
    *this = SavingThrows(0,0,0);
}

//Define SavingThrows.addSaves
SavingThrows SavingThrows::sumSaves(SavingThrows other){
    return SavingThrows(this->fort + other.fort, this->ref + other.ref, this->will + other.will);
}


//Function to get a good or bad save bonus based on HD
int saveBonus(bool goodSave, int HD){
    if (goodSave) return 2 + (HD/2);
    else return (HD/3);
}

//Return letters representing a goodSaveNum value
std::string saveLetters(int num){
    switch (num){
        case 1: return std::string("F");
        case 2: return std::string("R");
        case 3: return std::string("W");
        case 4: return std::string("FR");
        case 5: return std::string("FW");
        case 6: return std::string("RW");
        case 7: return std::string("FRW");
        default: return std::string("N");
    }
}

//Return whether or not a specific saving throw type is good based on the goodSaves value of a creature type/class
bool goodSave(int type, int goodSaves){
    //Type must be 1 (F), 2 (R), or 3 (W)
    switch (type){
        //Fortitude
        case 1:
            return (goodSaves == 1 || goodSaves == 4 || goodSaves == 5 || goodSaves == 7);
            break;
        //Reflex
        case 2:
            return (goodSaves == 2 || goodSaves == 4 || goodSaves == 6 || goodSaves == 7);
            break;
        //Will
        case 3:
            return (goodSaves == 3 || goodSaves == 5 || goodSaves == 6 || goodSaves == 7);
            break;
        default:
            return false;
    }
}

//Get the saving throw bonus for all three saving throws as a SavingThrows object based on HD and good saves
SavingThrows saveBonuses(int goodSaves, int HD){
    return SavingThrows(saveBonus(goodSave(F, goodSaves), HD), saveBonus(goodSave(R, goodSaves), HD), saveBonus(goodSave(W, goodSaves), HD));
}


//Ability score stuff
//AScores blank constructor
AScores::AScores(){
    *this = AScores(10,10,10,10,10,10);
}

//AScores constructor
AScores::AScores(int strength, int dexterity, int constitution, int intelligence, int wisdom, int charisma){
    this->scores[0] = strength;
    this->scores[1] = dexterity;
    this->scores[2] = constitution;
    this->scores[3] = intelligence;
    this->scores[4] = wisdom;
    this->scores[5] = charisma;
}

//Get a modifier for an ability score
int AScores::getMod(int ability){
    //Return 0 for an invalid ability score
    if (ability < 0 || ability > 5) return 0;

    //Otherwise, return the calculated modifier
    int score = this->scores[ability];
    return (score >= 10 ? (score - 10) / 2 : 0 - ((11 - score) / 2) );
}


//Sourcebook list, formatted as "abbreviation,fullname;"
std::vector<std::string> bookShort;
std::vector<std::string> bookLong;

//Add sourcebook to vectors
void addBook(const char* abbr, const char* name){
    bookShort.emplace_back(abbr);
    bookLong.emplace_back(name);
}

//Populate sourcebook lists
void addBookList(){
    addBook("PHB", "Player's Handbook");
    addBook("EPH", "Expanded Psionics Handbook");
    addBook("MM", "Monster Manual");
    addBook("ROD", "Races of Destiny");
};

//Get sourcebook name or abbreviation from the sourcebook list
std::string bookAbbr(int i){
    if (i >= (int) bookShort.size() || i < 0) return std::string("[X]");
    return bookShort.at(i);
}
std::string bookName(int i){
    if (i >= (int) bookLong.size() || i < 0) return std::string("[Invalid Book]");
    return bookLong.at(i);
}


//Roll a die of the given size. Default to a d20.
int rollDie(int dieSize){
    return (rand() % dieSize) + 1;
}
int rollDie(){
    return rollDie(20);
}


//Call data initialisation functions
int init(){
    //Initialise sourcebook list (35Manager.cc)
    addBookList();

    //Initialise skill list (skillList.cc)
    addSkillList();

    //Initialise creature type list (typeList.cc)
    std::vector<CreatureType>* types = addCtypes();

    //Initialise Knowledge skill-creature type association list (typeList.cc)
    addKtypes();

    //Initialise race list (raceList.cc)
    addCRaces();

    //Initialise class list (including creature types from addCtypes) (classList.cc)
    addCClasses(*types);

    //Initialise all the groups in the Characters directory
    addGroups();

    return 0;
}

//Total lines of code: 2998 (including whitespace)

int main(int argc, char** argv){
    //Initialise data
    init();

    //printf("Hello, Greyhawk!\n\n");
    terminal();
    
    return 0;
}
