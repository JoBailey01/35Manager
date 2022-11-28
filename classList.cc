#include "35Manager.h"

//CClass constructor
CClass::CClass(bool isType, const char* name, int goodSaves, int HD, int BAB, int sourcebook, int page){
    this->isType = isType;
    this->name = std::string(name);

    //An invalid number here, like -1, just means no good saves. This only occurs for types as classes, not for proper character classes.
    this->goodSaves = goodSaves;

    this->HD = checkDieSize(HD);
    this->BAB = checkBAB(BAB);

    this->sourcebook = sourcebook;
    this->page = page;
}

//Full description of a class
std::string CClass::fullDescription(){
    std::string output = "";

    return output + (this->isType ? "[Type] " : "") + this->name + " [d" + std::to_string(this->HD) + "]" +
        " (" + bookAbbr(this->sourcebook) + " " + std::to_string(this->page) + ")" +
        "\n\tBAB: " + getBABProgression(this->BAB) + ", Good Saves: " + saveLetters(this->goodSaves);
}

//Dummy (invalid) class
CClass noCClass = CClass(false, "[Invalid]", -1, -1, -1, -1, -1);

//CClasses vector
std::vector<CClass> classes;

//Find a CClass in the classes vector
CClass* findClass(const char* name){
    std::string inName = toLower(name);

    //Iterate over classes
    for (int i = 0;i < (int) classes.size(); i++){
        if (inName.compare(toLower(classes.at(i).name)) == 0) return &(classes.at(i));
    }

    return &noCClass;
}

//Variable to simplify adding elements to the classes list with add()
bool addType = true;

//Add a single element to the classes vector, using isType variable for the isType field
int add(const char* name, int goodSaves, int HD, int BAB, int sourcebook, int page){
    classes.emplace_back(addType, name, goodSaves, HD, BAB, sourcebook, page);
    
    return 0;
}

//List all classes in the classes vector, possibly including creature type classes
std::string listClasses(bool listTypes){
    std::string output = "";

    //Iterate over classes
    for (CClass c : classes){
        //Only display type-classes if listTypes is true
        if (listTypes || !c.isType){
            output += c.fullDescription() + "\n";
        }
    }

    //Remove final "\n"
    if (output.size() > 0) output.pop_back();

    return output;
}

//Add all the classes, including creature types, to the classes vector
int addCClasses(std::vector<CreatureType> types){
    //First, add creature type classes
    for (CreatureType t : types){
        if (!t.isSubtype){
            add(t.name.c_str(), t.goodSaves, t.HD, t.BAB, t.sourcebook, t.page);
        }
    }

    //Then, add standard character classes
    addType = false;

    //PHB
    add("Barbarian", F, 12, 4, PHB, 25);
    add("Bard", RW, 6, 3, PHB, 27);
    add("Cleric", FW, 8, 3, PHB, 31);
    add("Druid", FW, 8, 3, PHB, 35);
    add("Fighter", F, 10, 4, PHB, 39);
    add("Monk", FRW, 8, 3, PHB, 40);
    add("Paladin", F, 10, 4, PHB, 43);
    add("Ranger", FR, 8, 4, PHB, 46);
    add("Rogue", R, 6, 3, PHB, 49);
    add("Sorcerer", W, 4, 2, PHB, 52);
    add("Wizard", W, 4, 2, PHB, 55);

    return 0;
}

