#include "35Manager.h"

//Constructor for CreatureType
CreatureType::CreatureType(const char* name, int HD, int BAB, const char* description, int goodSaves, int sourcebook, int page, bool isSubtype){
    this->name = std::string(name);
    
    this->isSubtype = isSubtype;

    //Subtypes do not have HD, BAB, or saving throw progression
    if (!isSubtype){
        //Ensure valid HD size. Default to d8.
        this->HD = checkDieSize(HD);

        //BAB progression. Must be 2, 3, or 4.
        this->BAB = checkBAB(BAB);

        //Good saving throws. Since anything below 1 and above 7 is 'no good saves', any value is acceptable here.
        this->goodSaves = goodSaves;
    } else {
        this->HD = 0;
        this->BAB = 0;
        this->goodSaves = 0;
    }
    this->sourcebook = sourcebook;
    this->page = page;

    this->description = std::string(description);
}

//Get the full, extended description of this creature type
std::string CreatureType::fullDescription(){

    return this->getName() + " [d" + std::to_string(this->HD) + "] (" + bookAbbr(this->sourcebook) + " " + std::to_string(this->page) + ")" + "\n\t" +
        "BAB: " + getBABProgression(this->BAB) + ", Good Saves: " + saveLetters(this->goodSaves) +
        (this->description.size() > 0 ? "\n\t" + this->description : "");
}

//Get the name of this creature type, including () for subtype names
std::string CreatureType::getName(){
    return (this->isSubtype?"(":"") + this->name + (this->isSubtype?")":"");
}

//A vector containing all the creature types
std::vector<CreatureType> types;

//Dummy CreatureType object
CreatureType noCtype = CreatureType("[Invalid]", -1, -1, "[No description]", 0, -1, -1, false);

//Constructor for TypeKnowledge
TypeKnowledge::TypeKnowledge(Skill* knowledgeSkill, std::vector<CreatureType> types){
    this->knowledgeSkill = knowledgeSkill;
    this->types = types;
};

//Vector to store all knowledge-type associations
std::vector<TypeKnowledge> ktypes;

//Dummy TypeKnowledge object (set to the Appraise skill)
TypeKnowledge noKtype = TypeKnowledge(NULL, std::vector<CreatureType>());

//Find a TypeKnowledge object based on its name or alias
TypeKnowledge* findKnowledge(const char* knowledgeName){
    //Get the lowercase version of the name input
    std::string inAlias = toLower(knowledgeName);
    
    //Iterate over the list of TypeKnowledge instances
    for (int i = 0;i < (int) ktypes.size();i++){
        std::vector<std::string> aliases = (ktypes.at(i).knowledgeSkill)->alias;

        //Iterate over the aliases for this specific TypeKnowledge's associated Skill
        for (std::string s : aliases){
            if (s.compare(inAlias) == 0) return &(ktypes.at(i));
        }
    }

    return &noKtype;
}

//List the creature types assocated with a given Knowledge type
std::string TypeKnowledge::getCreatureTypes(){
    std::string output = "";

    //Iterate over creature types
    for (CreatureType t : this->types){
        output = output + t.getName() + ", ";
    }
    //Remove final ", "
    output.pop_back();
    output.pop_back();

    return output;
}

//Find a creature type's Knowledge skill object based on its name. Return default on failure.
Skill* findKtype(const char* creatureTypeName){
    //Lowercase form of creature type's name
    std::string name = toLower(creatureTypeName);

    //Iterate over the ktypes
    for (int i = 0;i < (int) ktypes.size();i++){
        TypeKnowledge cur = ktypes.at(i);

        //Iterate over the creature types for that ktype
        for (CreatureType t : cur.types){
            if (toLower(t.name).compare(name) == 0) return (ktypes.at(i).knowledgeSkill);
        }
    }

    return (noKtype.knowledgeSkill);
}

//Find a creature type in the types vector based on its name
CreatureType* findCreatureType(const char* creatureTypeName){
    std::string name = toLower(creatureTypeName);

    //Iterate over all known creature types
    for (int i = 0;i < (int) types.size(); i++){
        if (toLower(types.at(i).name).compare(name) == 0) return &(types.at(i));
    }

    return &noCtype;
}

//Add a new TypeKnowledge object to the ktypes vector
int addK(const char* skill, const char* types){
    //Find the appropriate skill object (should be a Knowledge skill)
    Skill* kSkill = findSkill(skill);

    //If the input skill is not a Knowledge skill, stop
    if(!(kSkill->isChild && (kSkill->parent != -1 and kSkill->getParent()->name.compare("Knowledge") == 0))){
        printf("> Warning: Non-Knowledge skill associated with creature types\n");
        return 1;
    }

    //If the input skill is already being used by an existing TypeKnowledge, stop
    for (TypeKnowledge t : ktypes){
        if (t.knowledgeSkill == kSkill){
            printf("> Warning: Knowledge skill %s is already has associated creature types\n", kSkill->getName().c_str());
            return 1;
        };
    }

    //Otherwise, assemble a vector of all the creature types from the comma-separated types input list
    std::vector<CreatureType> allTypes;

    //Iterate over types input
    const char* cur = types;
    std::string curTypeName = "";
    while(1){
        //Add a character to curTypeName or add an entire creature type to allTypes
        if (*cur != ',' && *cur != '\0'){
            curTypeName.push_back(*cur);
            cur++;
        } else {
            CreatureType temp = *findCreatureType(curTypeName.c_str());
            if (&temp != &noCtype) allTypes.push_back(temp);
            curTypeName.erase();

            if (*cur == '\0') break;
            cur++;
        }
    }

    //Add finished TypeKnowledge to ktypes vector
    ktypes.emplace_back(kSkill, allTypes);
    return 0;
}

//Add a new CreatureType object to the types vector
int addC(const char* name, int HD, int BAB, const char* description, int goodSaves, int sourcebook, int page, bool isSubtype){
    //Search name
    std::string lowName = toLower(name);
    
    //If this name is already in use, stop
    for (CreatureType t : types){
        if (toLower(t.name).compare(lowName) == 0){
            printf("> Warning: Double declaration of creature type\n");
            return 1;
        }
    }

    types.emplace_back(name, HD, BAB, description, goodSaves, sourcebook, page, isSubtype);
    
    return 0;
}


//List all knowledge skills and their associated creature types
std::string listKnowledge(){
    std::string output = "";
    int longestName = 0;

    //Find the longest Knowledge skill name
    for (TypeKnowledge k : ktypes){
        if ((int) k.knowledgeSkill->name.size() > longestName) longestName = (int) k.knowledgeSkill->name.size();
    }

    //Iterate over ktypes
    for (TypeKnowledge k : ktypes){
        std::string curName = k.knowledgeSkill->name;
        
        //Add spaces to output to even the names
        for (int i = (int) curName.size();i < longestName;i++){
            output.push_back(' ');
        }

        output += curName + " | " + k.getCreatureTypes() + "\n";
    }

    return output;
}

//Enums for creature types: subtype or non-subtype
enum ctypeLevel{subtype = true, fulltype = false};

//Function to populate types list with all the creature types
std::vector<CreatureType>* addCtypes(){
    //We don't actually need all the subtypes just for Knowledge skills, but I've added them in case I want them later
    //Subtypes don't have HD or BAB progression; the constructor automatically removes them
    
    addC("Aberration", 8, 3, "", W, MM, 305, fulltype);
        addC("Air", 8, 3, "", N, MM, 305, subtype);
        addC("Angel", 8, 3, "", N, MM, 305, subtype);
    addC("Animal", 8, 3, "", FR, MM, 305, fulltype);
        addC("Aquatic", 3, 3, "", N, MM, 306, subtype);
        addC("Archon", 8, 3, "", N, MM, 306, subtype);
        addC("Augmented", 8, 3, "Augmented [original base type] is applied to creatures that have changed type due to a template", N, MM, 306, subtype);
        addC("Baatezu", 8, 3, "Devil (not exclusive)", N, MM, 306, subtype);
        addC("Chaotic", 8, 3, "", N, MM, 307, subtype);
        addC("Cold", 8, 3, "", N, MM, 307, subtype);
    addC("Construct", 10, 3, "", N, MM, 307, fulltype);
    addC("Dragon", 12, 4, "", FRW, MM, 308, fulltype);
        addC("Dwarf", 8, 3, "", N, PHB, 1, subtype);
        addC("Earth", 8, 3, "", N, MM, 308, subtype);
        addC("Eladrin", 8, 3, "", N, MM, 308, subtype);
    addC("Elemental", 8, 3, "Earth/water have good Fortitude saves, air/fire have good Reflex saves", N, MM, 308, fulltype);
        addC("Elf", 8, 3, "", N, PHB, 1, subtype);
        addC("Evil", 8, 3, "", N, MM, 308, subtype);
        addC("Extraplanar", 8, 3, "", N, MM, 309, subtype);
    addC("Fey", 6, 2, "", RW, MM, 309, fulltype);
        addC("Fire", 8, 3, "", N, MM, 309, subtype);
    addC("Giant", 8, 3, "", F, MM, 310, fulltype);
        addC("Gnome", 8, 3, "", N, PHB, 1, subtype);
        addC("Goblinoid", 8, 3, "", N, MM, 310, subtype);
        addC("Good", 8, 3, "", N, MM, 310, subtype);
        addC("Guardinal", 8, 3, "", N, MM, 310, subtype);
        addC("Halfling", 8, 3, "", N, PHB, 1, subtype);
    addC("Humanoid", 8, 3, "", N, MM, 310, fulltype);
        addC("Human", 8, 3, "", N, PHB, 1, subtype);
        addC("Incorporeal", 8, 3, "", N, MM, 310, subtype);
        addC("Lawful", 8, 3, "", N, MM, 311, subtype);
    addC("Magical Beast", 10, 4, "", FR, MM, 311, fulltype);
        addC("Maenad", 8, 3, "", N, EPH, 1, subtype);
    addC("Monstrous Humanoid", 8, 4, "", RW, MM, 311, fulltype);
        addC("Native", 8, 3, "", N, MM, 312, subtype);
    addC("Ooze", 10, 3, "", N, MM, 313, fulltype);
        addC("Orc", 8, 3, "", N, PHB, 1, subtype);
    addC("Outsider", 8, 4, "", FRW, MM, 313, fulltype);
    addC("Plant", 8, 3, "", F, MM, 313, fulltype);
        addC("Psionic", 8, 3, "", N, EPH, 183, subtype);
        addC("Reptilian", 8, 3, "", N, MM, 314, subtype);
        addC("Shapechanger", 8, 3, "", N, MM, 314, subtype);
        addC("Swarm", 8, 3, "", N, MM, 315, subtype);
        addC("Tanar'ri", 8, 3, "Demon (not exclusive)", N, MM, 316, subtype);
    addC("Undead", 12, 2, "", W, MM, 317, fulltype);
    addC("Vermin", 8, 3, "", F, MM, 317, fulltype);
        addC("Water", 8, 3, "", N, MM, 317, subtype);
        addC("Xeph", 8, 3, "", N, EPH, 1, subtype);
    
    return &types;
}

//Function to populate ktypes list with knowledge-type associations
int addKtypes(){
    noKtype.knowledgeSkill = findSkill("app");

    addK("k Arcana", "construct,dragon,magical beast");
    addK("k Dungeoneering", "aberration,ooze");
    addK("k Local", "humanoid");
    addK("k Nature", "animal,fey,giant,monstrous humanoid,plant,vermin");
    addK("k Religion", "undead");
    addK("k Planes", "outsider,elemental");
    addK("k Psionics", "psionic");

    return 0;
}

