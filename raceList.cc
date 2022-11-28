#include "35Manager.h"

//Define constructor for CRace
CRace::CRace(const char* name, int LA, CreatureType* type, std::vector<CreatureType> subtypes, int RHD, int sourcebook, int page){
    this->name = std::string(name);
    
    //LA and RHD must be positive
    this->LA = LA >= 0 ? LA : 0;
    this->RHD = RHD >= 0 ? RHD : 0;

    this->sourcebook = sourcebook;
    this->page = page;
    
    this->type = type;
    this->subtypes = std::vector<CreatureType>();
    
    //Remove non-subtypes from the subtypes vector and add the rest to subtypes
    for (CreatureType s : subtypes){
        if (s.isSubtype) this->subtypes.push_back(s);
    }
}

//Get the full description text of a CRace
std::string CRace::fullDescription(){
    //Output string
    std::string output = "";

    //Assemble subtype list in a string
    std::string subtypes = "";
    if (this->subtypes.size() > 0){
        subtypes += " (";
        for (CreatureType t : this->subtypes){
            subtypes += t.name + ", ";
        }
        subtypes.pop_back();
        subtypes.pop_back();
        subtypes.push_back(')');
    }

    return output + this->name + " (" + bookAbbr(this->sourcebook) + " " + std::to_string(this->page) + ")" +
        ": " + this->type->getName() + subtypes +
        (this->LA > 0 ? (", LA +" + std::to_string(this->LA)) : "") +
        (this->RHD > 0 ? (", " + std::to_string(this->RHD) + " RHD") : "");;
}

//A vector of all race options
std::vector<CRace> CRaces;

//List all race options
std::string listRaces(){
    //Final output string
    std::string output = "";

    //Iterate over CRaces
    for (CRace c : CRaces){
        output += c.fullDescription() + "\n";
    }

    //Remove final line break
    output.pop_back();

    return output;
}

//Dummy race object
CRace noCRace = CRace("[Invalid]", 0, findCreatureType("Invalid"), std::vector<CreatureType>(), 0, -1, -1);

//Find a race by name
CRace* findCRace(const char* name){
    //Name as a lowercase std::string
    std::string inName = toLower(name);

    //Iterate over CRaces
    for (int i = 0;i < (int) CRaces.size();i++){
        if (inName.compare(toLower(CRaces.at(i).name)) == 0) return &(CRaces.at(i));
    }

    return &noCRace;
}

//Add an element to the races vector. subtypes is a comma-separated list WITHOUT parenthesised names.
int add(const char* name, int LA, const char* type, const char* subtypes, int RHD, int sourcebook, int page){
    //Find the appropriate creature type. If this is a subtype, stop.
    CreatureType* typeObj = findCreatureType(type);
    if (typeObj->isSubtype) return 1;

    //Find all the subtypes in the comma-separated list.
    std::string str = "";
    std::vector<CreatureType> subtypeList;
    while (1){
        if (*subtypes == ',' || *subtypes == '\0'){
            if (str.size() > 0) subtypeList.push_back(*findCreatureType(str.c_str()));
            if (*subtypes == '\0') break;
            str.erase();
            subtypes++;
        }
        str.push_back(*subtypes);
        subtypes++;
    }
    
    //Add completed race to races vector
    CRaces.emplace_back(name, LA, typeObj, subtypeList, RHD, sourcebook, page);

    return 0;
}

//Add all the races to the CRaces
int addCRaces(){
    add("Dromite", 1, "Monstrous Humanoid", "Psionic", 0, EPH, 194);
    add("Dwarf", 0, "Humanoid", "Dwarf", 0, MM, 91);
    add("Elf", 0, "Humanoid", "Elf", 0, MM, 101);
    add("Gnome", 0, "Humanoid", "Gnome", 0, MM, 131);
    add("Goblin", 0, "Humanoid", "Goblinoid", 0, MM, 133);
    add("Half-Elf", 0, "Humanoid", "Elf,Human", 0, PHB, 18);
    add("Half-Orc", 0, "Humanoid", "Human,Orc", 0, PHB, 18);
    add("Halfling", 0, "Humanoid", "Halfling", 0, MM, 149);
    add("Human", 0, "Humanoid", "Human", 0, PHB, 12);
    add("Illumian", 0, "Humanoid", "Human", 0, ROD, 53);
    add("Kobold", 0, "Humanoid", "Reptilian", 0, MM, 161);
    add("Locathah", 0, "Humanoid", "Aquatic", 0, MM, 169);
    add("Maenad", 0, "Humanoid", "Maenad,Psionic", 0, EPH, 203);
    add("Merfolk", 0, "Humanoid", "Aquatic", 0, MM, 185);
    add("Orc", 0, "Humanoid", "Orc", 0, MM, 203);
    add("Sahuagin", 0, "Monstrous Humanoid", "Aquatic", 0, MM, 217);
    add("Sea Kin", 0, "Humanoid", "Human", 0, ROD, 99);
    add("Sharakim", 1, "Humanoid", "Human", 0, ROD, 104);
    add("Xeph", 0, "Humanoid", "Psionic,Xeph", 0, EPH, 217);

    return 0;
}

