#include "35Manager.h"

//Convert a std::string to all-lowercase letters
std::string toLower(std::string input){
    std::string output = "";

    //Iterate over input string
    for (int i = 0;i < (int) input.length();i++){
        char cur = input.at(i);

        //Handle uppercase letters
        if (cur >= 65 && cur <= 90){
            cur += 32;
        }

        output.push_back(cur);
    }

    return output;
}

//Handle const char* input
std::string toLower(const char* input){
    return toLower(std::string(input));
}

//Define Skill.getName
std::string Skill::getName(){
    //Non-child skills
    if (!this->isChild){
        //Parent and non-parent skills
        if (this->isParent) return std::string("[" + this->name + "]");
        else return std::string(this->name);
    }

    //Child skills
    std::string parentName = this->getParent()->name;
    //printf("\t> %s (%s)\n", parentName.c_str(), this->name.c_str());
    return std::string(parentName + " (" + this->name + ")");
}

//Define Skill.longString
std::string Skill::longString(){
    return this->getName() + " | " + abilityAbbr(this->ability) + (this->isTrained || this->isACP ? " " : "") +
        (this->isTrained ? (this-> trainCap == 0 ? "[t]" : "[t " + std::to_string(this->trainCap) + "]") : "") +
        (this->isACP ? "[a]" : "") +
        " (" + bookAbbr(this->sourcebook) + " " + std::to_string(this->page) + ")"
        ;
}

//Define Skill.detailString
std::string Skill::detailString(char l){
    return std::string("") +
        '\n' + l + "Try Again: " + (tryAgain != 0 ? (tryAgain == 1 ? "No" : "Special") : "Yes") +
        '\n' + l + "Rolled by " + (DMRoll ? "DM" : "player") +
        '\n' + l + "Aliases: " + getAliases()
        ;
}

//Define Skill.getAliases
std::string Skill::getAliases(){
    std::string output = "";
    for (std::string s : alias){
        output += s + ",";
    }

    //Remove final ,
    output.pop_back();

    return output;
}

//Define constructor for Skill
Skill::Skill(const char* name, int ability, bool isTrained, int trainCap, bool isACP, bool isParent, int parent, int tryAgain, bool DMRoll, int sourcebook, int page, std::vector<std::string> aliases){

    this->name = std::string(name);
    this->ability = (ability >= 0 && ability <= 5) ? ability : 3; //Default ability score to 3 (INT) if input is invalid
    this->isTrained = isTrained;
    this->isACP = isACP;
    this->isParent = isParent;
    this->parent = parent;
    this->tryAgain = (tryAgain >= 0 && tryAgain <= 2) ? tryAgain : 0; //Default try again to 'yes' if input is invalid
    this->DMRoll = DMRoll;
    this->sourcebook = sourcebook;
    this->page = page;
    this->isChild = parent == -1 ? false : true;

    //Get a pointer to this object's parent, if applicable
    Skill* par = this->getParent();

    //Handle inherited values for child skills
    if(isChild) {
        this->ability = par->ability;
        this->tryAgain = par->tryAgain;
        this->DMRoll = par->DMRoll;
    }

    //Handle training check cap
    if (isChild) this->trainCap = par->trainCap;
    else if (isTrained) this->trainCap = trainCap;
    else this->trainCap = 0;

    //Add the skill's standalone name to the aliases vector
    alias.push_back(toLower(this->name));

    //Add abbreviated forms of the parent-child names (e.g., k arcana,k arc) for each alias
    if (this->isChild){
        int aliasSize = (int) aliases.size();
        std::string parentStart = par->name.substr(0,1);
        for (int i = 0;i < aliasSize;i++){
            alias.push_back(toLower(parentStart + " " + aliases.at(i)));
        }
        
        alias.push_back(toLower(parentStart + " " + this->name));

        //Add the skill's parent-child name to the aliases vector (e.g, Knowledge (Arcana))
        alias.push_back(toLower(this->getName()));
    } else {
        //Add all other aliases to the aliases vector, converting to all-lowercase characters
        for (int i = 0;i < (int) aliases.size(); i++) {
            alias.push_back(toLower(aliases.at(i)));
        }
    }
}

//Define blank constructor for Skill. This creates a blank dummy value.
Skill blankSkill(){
    return Skill("[Invalid Skill]", STR, false, 0, false, false, -1, 0, false, -1, -1, std::vector<std::string>());
}

//A default blank dummy skill
Skill noSkill = blankSkill();

//A vector containing all the skills in an ordered list
std::vector<Skill> skills;

//Get a current pointer to the parent object of a child skill, or noSkill if the skill object is not a child
Skill* Skill::getParent(){
    if (this->isChild) return &(skills.at(this->parent));
    else return &noSkill;
}

//List all the active skills in the skills list
std::string listSkills(bool detail){
    std::string output;

    //Iterate over all the skills
    for (int i = 0;i < (int) skills.size();i++){
        Skill s = skills.at(i);
        if (s.isParent) continue;
        output += s.longString();
        if (detail) output += s.detailString('\t');
        output.push_back('\n');
    }

    return output;
}


//Find a skill's index based on its alias. Default to -1. This operation is currently O(n * m), where m is the number of aliases each skill has.
int findSkillIndex(const char* inAlias){
    //Handle null input
    if (inAlias == NULL) return -1;
    
    std::string alias = toLower(inAlias);

    //Iterate over all the skills in the skills list
    for (int i = 0;i < (int) skills.size();i++){
        Skill cur = skills.at(i);

        //Iterate over each alias of this skill
        for (int j = 0;j < (int) cur.alias.size(); j++){
            if (cur.alias.at(j).compare(alias) == 0) return i;
        }
    }

    //No matching skill found
    return -1;
}

//Define findSkill. This operation is currently O(n * m), where m is the number of aliases each skill has.
Skill* findSkill(const char* inAlias){
    int index = findSkillIndex(inAlias);
    return index != -1 ? &(skills.at(index)) : &noSkill;
}

//Add a single skill to the skills vector. Takes a comma-separated list of aliases. Return 0 for success.
int add(const char* name, int ability, bool isTrained, int trainCap, bool isACP, bool isParent, const char* parent, int tryAgain, bool DMRoll, int sourcebook, int page, const char* aliasList){
    //Convert the null-terminated aliases array into strings and add then to a vector
    std::vector<std::string> aliases;

    //Handle the comma-separated alias list input
    int len = aliasList == NULL ? 0 : strlen(aliasList);
    if (len > 0){
        std::string curAlias;
        for (int i = 0;i < len; i++){
            //Add characters to the list one by one, creating new words on a comma
            char cur = *aliasList;
            if (cur == ','){
                aliases.push_back(curAlias);
                curAlias.clear();
            } else {
                curAlias.push_back(cur);
            }

            aliasList++;
        }
        aliases.push_back(curAlias);
    }

    //Find the parent, if applicable
    int parentIndex = findSkillIndex(parent);
        
    //Skill(const char* name, int ability, bool isParent, Skill* parent, std::vector<std::string> aliases);
    //Return final Skill object
    skills.emplace_back(name, ability, isTrained, trainCap, isACP, isParent, parentIndex, tryAgain, DMRoll, sourcebook, page, aliases);

    return 0;
}

//Enums to make add() calls more readable
enum addVals {untrained=false, trained=true, parent=true, nonparent=false, ACP=true, noACP=false, DM=true, PC=false};
enum tryVals {tryAgain=0, tryNot=1, trySpecial=2};

//Define all the skills in the game. Return 0 for success.
int addSkillList(){
    //name, ability, isTrained, isACP, trainCap, isParent, parent, aliasList
    add("Appraise", INT, untrained, 0, noACP, nonparent, NULL, tryNot, PC, PHB, 67, "app");
    add("Autohypnosis", WIS, trained, 0, noACP, nonparent, NULL, trySpecial, PC, EPH, 36, "auto,aut,ah,hyp");
    add("Balance", DEX, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 67, "bal");
    add("Bluff", CHA, untrained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 67, "blu");
    add("Climb", STR, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 69, "cli");
    add("Concentration", CON, untrained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 69, "conc,con");

    add("Craft", INT, untrained, 0, noACP, parent, NULL, trySpecial, PC, PHB, 70, "");
        add("Alchemy", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "alc");
        add("Armoursmithing", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "armour,armsm");
        add("Basketweaving", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "basket,bask,baskw");
        add("Bookbinding", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "book,bookb");
        add("Bowmaking", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "bow,bowm");
        add("Blacksmithing", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "bla,blasm,blsm");
        add("Calligraphy", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "cal,calg");
        add("Carpentry", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "car,carp");
        add("Cobbling", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "cob,cobb");
        add("Gemcutting", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "gem,gemc");
        add("Leatherworking", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "leather,lea,leaw");
        add("Locksmithing", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "lock,locksm");
        add("Painting", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "pai,paint");
        add("Pottery", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "pot,pott");
        add("Sculpting", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "sculpture,scu,scul");
        add("Shipmaking", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "ship,shipm");
        add("Stonemasonry", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "stone,sto,stonem");
        add("Trapmaking", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "trap,trapm");
        add("Weaponsmithing", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "weapon,wea,weasm");
        add("Weaving", INT, untrained, 0, noACP, nonparent, "craft", -1, PC, PHB, 70, "weave");
    
    add("Decipher Script", INT, trained, 0, noACP, nonparent, NULL, tryNot, DM, PHB, 71, "ds,decipher,decscript");
    add("Diplomacy", CHA, untrained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 71, "dip,diplo");
    add("Disable Device", INT, trained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 72, "dd,disable,disdevice,disdev");
    add("Disguise", CHA, untrained, 0, noACP, nonparent, NULL, tryAgain, PC, PHB, 72, "dis");
    add("Escape Artist", DEX, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 73, "ea,escape,escart");
    add("Forgery", INT, untrained, 0, noACP, nonparent, NULL, tryNot, DM, PHB, 74, "for,forge");
    add("Gather Information", CHA, untrained, 0, noACP, nonparent, NULL, tryAgain, PC, PHB, 74, "gi,gather,gatinf,ginfo");
    add("Handle Animal", CHA, trained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 74, "ha,handle,hananimal");
    add("Heal", WIS, untrained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 75, "");
    add("Hide", DEX, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 76, "");
    add("Intimidate", CHA, untrained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 76, "intim,int");
    add("Jump", STR, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 77, "");

    add("Knowledge", INT, trained, 10, noACP, parent, NULL, tryNot, PC, PHB, 78, "");
        add("Arcana", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "arc");
        add("Architecture and Engineering", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "ae,aae,a&e,a/e,architecture,engineering,arch,eng");
        add("Dungeoneering", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "dun,dungeon");
        add("Geography", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "geo");
        add("History", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "hist");
        add("Local", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "loc");
        add("Nature", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "nat");
        add("Nobility and Royalty", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "nr,nar,n&r,n/r,nobility,royalty,nob,roy");
        add("The Planes", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "planes,pla");
        add("Psionics", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, EPH, 38, "psi");
        add("Religion", INT, trained, 10, noACP, nonparent, "knowledge", -1, PC, PHB, 78, "rel,relig");
    
    add("Listen", WIS, untrained, 0, noACP, nonparent, NULL, tryAgain, DM, PHB, 78, "lis,l");
    add("Move Silently", DEX, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 79, "ms,move,silently,moves,movsil");
    add("Open Lock", DEX, trained, 0, noACP, nonparent, NULL, tryAgain, DM, PHB, 77, "ol,open,openl,opelock");

    add("Perform", CHA, untrained, 0, noACP, parent, NULL, trySpecial, PC, PHB, 79, "");
        add("Act", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "acting");
        add("Comedy", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "com");
        add("Dance", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "dan");
        add("Keyboard", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "key,kb");
        add("Oratory", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "ora,orat");
        add("Percussion", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "per,perc");
        add("String Instrument", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "si,string,strings,strinst");
        add("Wind Instrument", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "wi,wind,woodwind,wininst,windinst");
        add("Sing", CHA, untrained, 0, noACP, nonparent, "perform", -1, PC, PHB, 79, "vocal");
    
    add("Profession", WIS, trained, 0, noACP, parent, NULL, trySpecial, PC, PHB, 80, "");
        add("Apothecary", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "apo,apoth");
        add("Boater", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "boa,boat");
        add("Bookkeeper", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "boo,book");
        add("Brewer", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "bre,brew");
        add("Cook", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "coo");
        add("Driver", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "dri,drive");
        add("Farmer", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "far,farm");
        add("Fisher", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "fis,fish");
        add("Guide", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "gui");
        add("Herbalist", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "herb");
        add("Herder", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "herd");
        add("Hunter", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "hun,hunt");
        add("Innkeeper", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "inn,innk");
        add("Lumberjack", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "lum,lumber,lumberj");
        add("Miller", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "mil,mill");
        add("Miner", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "min,mine");
        add("Porter", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "por,port");
        add("Rancher", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "ran,ranch");
        add("Sailor", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "sai,sail");
        add("Scribe", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "scr");
        add("Siege Engineer", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "sie,siege,siegeeng,seng");
        add("Stablehand", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "sta,stable,stableh");
        add("Tanner", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "tan");
        add("Teamster", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "tea,team");
        add("Woodcutter", WIS, trained, 0, noACP, nonparent, "profession", -1, PC, PHB, 80, "woo,wood,woodc");
    
    add("Psicraft", INT, trained, 0, noACP, nonparent, NULL, trySpecial, PC, EPH, 38, "psi,psic");
    add("Ride", DEX, untrained, 0, noACP, nonparent, NULL, tryAgain, PC, PHB, 80, "rid");
    add("Search", INT, untrained, 0, noACP, nonparent, NULL, tryAgain, PC, PHB, 81, "sea");
    add("Sense Motive", WIS, untrained, 0, noACP, nonparent, NULL, tryNot, PC, PHB, 81, "sm,sense,motive");
    add("Sleight of Hand", DEX, trained, 0, ACP, nonparent, NULL, trySpecial, PC, PHB, 81, "soh,sleight");
    
    add("Speak Language", INT, trained, 0, noACP, parent, NULL, tryNot, PC, PHB, 82, "sl,speak,lang");
        add("Onyel (Elven)", INT, trained, 0, noACP, nonparent, "sl", -1, PC, PHB, 82, "onyel,elven");
        add("Wajiteri (Uadien/Wadian)", INT, trained, 0, noACP, nonparent, "sl", -1, PC, PHB, 82, "wajiteri,uadien,wadian");
        add("Dzafreki (Dwarven)", INT, trained, 0, noACP, nonparent, "sl", -1, PC, PHB, 82, "dzafreki,dwarven");
        //TODO: Add remaining languages
    
    add("Spellcraft", INT, trained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 82, "spe,spell,spellc");
    add("Spot", WIS, untrained, 0, noACP, nonparent, NULL, tryAgain, DM, PHB, 83, "s,spo");
    add("Survival", WIS, untrained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 83, "sur,survive");
    add("Swim", STR, untrained, 0, ACP, nonparent, NULL, tryAgain, PC, PHB, 84, "swi");
    add("Tumble", DEX, trained, 0, ACP, nonparent, NULL, tryNot, PC, PHB, 84, "tum");
    add("Use Magic Device", CHA, trained, 0, noACP, nonparent, NULL, trySpecial, PC, PHB, 85, "umd");
    add("Use Psionic Device", CHA, trained, 0, noACP, nonparent, NULL, trySpecial, PC, EPH, 38, "upd");
    add("Use Rope", DEX, untrained, 0, noACP, nonparent, NULL, tryAgain, PC, PHB, 86, "ur,user,rope");

    return 0;
}

