#include "35Manager.h"

//A vector of all character groups
std::vector<Group> groups;

//Find a group based on an alias, or NULL if no such group exists
Group* findGroup(const char* inAlias){
    //Lowercase alias
    std::string alias = toLower(inAlias);

    //Iterate over groups vector
    for (int i = 0;i < (int) groups.size();i++){
        //Iterate over aliases of each group
        for (std::string a : groups.at(i).aliases){
            if (alias.compare(toLower(a)) == 0) return &(groups.at(i));
        }
    }

    return NULL;
}


//Generate the full descriptions of all the members of a group, with or without skill ranks, with or without skill bonuses
std::string Group::fullDescription(bool withRanks, bool withBonuses){
    std::string output = 
              "========= " + this->name + " =========\n";
    
    output += " | (a.k.a. ";
    for (std::string a : this->aliases){
        output += a + ", ";
    }
    output.pop_back();
    output.pop_back();
    output += ")\n";

    output += "==== Notes for " + this->name + " ====\n";

    //Add notes, with "   | " after every newline or after every MAXLEN characters
    int lineLen = 0;
    output+= " | ";
    for (int i = 0;i < (int) this->notes.size();i++){
        char c = this->notes.at(i);
        if (c=='\n' || lineLen >= MAXLEN){
            output += "\n | ";
            if (c != '\n') output.push_back(c);
            lineLen = 0;
        }
        else{
            output.push_back(c);
            lineLen++;
        }
    }
    output+="\n";

    output += "== Characters in " + this->name + " ==\n";
    
    //Iterate over members list
    for (LChar c : this->members){
        printf("+> %s\n", c.name.c_str());

        output += " + " + c.fullDescription(withRanks, withBonuses) + "\n\n";
    }

    output += "====== [End " + this->name + "] ======\n";

    return output;
}

//Generate a group's full description without skill ranks or bonuses
std::string Group::fullDescription(){
    return this->fullDescription(false, false);
}

//Read a group object in from some subdirectory of ./Characters
Group::Group(const char* directory, bool isParty){
    //Remove final / from directory name, if applicable
    std::string dirName = std::string(directory);
    if (dirName.back() == '/') dirName.pop_back();

    //Get the full relative directory name
    std::string tempPath = "./Characters/" + dirName;
    const char* dirPath = tempPath.c_str();

    //Make sure this directory exists
    DIR* dir = opendir(dirPath);
    if (dir==NULL){
        this->name = std::string("[Invalid Group]");
        this->isParty = false;
        this->members = std::vector<LChar>();
        this->notes = std::string("\"") + directory + "\" is not a valid directory in Characters.";
        return;
    }

    //Check for a name.txt file in this folder
    std::string tempNamePath = std::string(dirPath) + "/name.txt";
    const char* namePath = tempNamePath.c_str();
    FILE* nameFile = fopen(namePath, "r");

    //The name of this group is the name of the directory by default
    if (nameFile==NULL){
        this->name = dirName;
        this->aliases.emplace_back(dirName);
    //Read a name and aliases
    } else {
        this->name = "";
        char c = fgetc(nameFile);
        //Name
        while (c != EOF && c != '\n'){
            this->name.push_back(c);
            c = fgetc(nameFile);
        }

        //Aliases
        this->aliases.push_back(this->name);
        std::string curAlias = "";
        c = fgetc(nameFile);
        while(c != EOF){
            if (c == '\n'){
                this->aliases.emplace_back(curAlias);
                curAlias.erase();
                c = fgetc(nameFile);
            } else {
                curAlias.push_back(c);
                c = fgetc(nameFile);
            }
        }
        if (curAlias.size() > 0) this->aliases.emplace_back(curAlias);

        fclose(nameFile);
    }

    //Set up log file path
    this->logPath = std::string(dirPath) + "/log.txt";
    
    //Check for a notes.txt file in this folder
    std::string tempNotePath = std::string(dirPath) + "/notes.txt";
    const char* notePath = tempNotePath.c_str();
    FILE* notes = fopen(notePath, "r");

    //Handle a missing notes file
    if (notes==NULL){
        this->notes = "";
    //Notes file
    } else {
        this->notes = "";
        char c = fgetc(notes);
        while (c != EOF){
            this->notes.push_back(c);
            c = fgetc(notes);
        }

        fclose(notes);
    }

    //Now, read in all the .ddc files and add them to the members array
    while (1){
        //Read in the next file
        struct dirent* curEntry = readdir(dir);

        //If there is an error or this is the last file, stop
        if (curEntry == NULL) break;

        //Get a LChar for this file.
        std::string tempCharPath = std::string(dirPath) + "/" + curEntry->d_name;
        LChar* temp = readCharacter(tempCharPath.c_str());

        //Skip invalid characters
        if (temp==NULL){
            continue;
        }

        //Otherwise, save this character to the group's members vector
        this->members.emplace_back(*temp);
    }

    //Insertion-sort the members array alphabetically by name
    for (int i = 1;i < (int) this->members.size();i++){
        int j = i-1;
        while (j >=0 && this->members.at(j).name.compare(this->members.at(i).name) > 0){
            LChar temp = this->members.at(j+1);
            this->members.at(j+1) = this->members.at(j);
            this->members.at(j) = temp;
            j--;
        }
    }

    closedir(dir);

}


//Group query methods
//Get the names of all group members as a multi-line list
std::string Group::queryNames(const char* lineStart){
    std::string output = "";
    for (LChar l : this->members){
        output += lineStart + l.name + "\n";
    }
    output.pop_back();

    return output;
}

//Get short descriptions of all group members, each on its own line
std::string Group::queryShortDescription(const char* lineStart){
    std::string output = "";
    for (LChar l : this->members){
        output += lineStart + l.shortDescription() + "\n";
    }
    output.pop_back();

    return output;
}

//Get long descriptions of all group members, with or without ranks, with or without bonuses
std::string Group::queryLongDescription(bool withRanks, bool withBonuses){
    std::string output = "";
    for (LChar l : this->members){
        output += l.fullDescription(withRanks, withBonuses) + "\n";
    }
    output.pop_back();

    return output;
}


//Roll saving throws for all group members based on save name (e.g., fort, fortitude, f)
std::string Group::querySaves(const char* lineStart, const char* saveType){
    std::string saveName = toLower(saveType);

    int type = -1;
    if(saveName.compare("fort") == 0 || saveName.compare("fortitude") == 0 || saveName.compare("f") == 0) type = 0;
    else if(saveName.compare("ref") == 0 || saveName.compare("reflex") == 0 || saveName.compare("r") == 0) type = 1;
    else if(saveName.compare("wil") == 0 || saveName.compare("will") == 0 || saveName.compare("w") == 0) type = 2;

    return this->querySaves(lineStart, type);
}

//Roll saving throws for all group members. If input number is invalid, (fort=0, ref=1, will=2), default to will
std::string Group::querySaves(const char* lineStart, int saveType){
    std::string output = "";

    //Get save name
    switch(saveType){
        case 0:
            output += "Fortitude";
            break;
        case 1:
            output += "Reflex";
            break;
        case 2:
        default:
            output += "Will";
            break;
    }

    output += " saving throws for " + this->name + "\n";

    //Iterate over group members
    for (LChar l : this->members){
        //Get applicable save bonus
        int mod = 0;
        switch (saveType){
            case 0:
                mod = l.saves.fort;
                break;
            case 1:
                mod = l.saves.ref;
                break;
            case 2:
            default:
                mod = l.saves.will;
                break;
        }

        //Roll die
        int die = rollDie();
        
        //Format example: "Name (+2): [14] 16"
        output += lineStart + l.name + 
            " (" + (mod >= 0 ? "+" : "") + std::to_string(mod) + "): [" +
            std::to_string(die) + "] " + std::to_string(die + mod) + "\n";
    }
    output.pop_back();

    return output;
}


//Roll skill checks for all group members
std::string Group::querySkillCheck(const char* lineStart, const char* skillName){
    std::string output = "";

    //Get skill pointer
    Skill* s = findSkill(skillName);

    output += s->getName() + " check for " + this->name + "\n";

    //Iterate over group members
    for (LChar l : this->members){
        //Only roll if this character can make this check
        if (!l.canUseSkill(s)) continue;

        //Get applicable modifier
        int mod = l.getBonus(s);

        //Roll die
        int die = rollDie();
        
        //Format example: "Name (+2): [14] 16"
        output += lineStart + l.name + 
            " (" + (mod >= 0 ? "+" : "") + std::to_string(mod) + "): [" +
            std::to_string(die) + "] " +
            (s->isTrained && l.getRanks(s) <= 0 ? (die + mod > s->trainCap ? std::to_string(s->trainCap) : std::to_string(die + mod)) : std::to_string(die + mod)) +
            "\n";
    }
    output.pop_back();

    return output;
}

//Roll the appropriate skill check for a given creature type
std::string Group::queryCreatureKnowledge(const char* lineStart, const char* ctype){
    return "[Creature Type: " + findCreatureType(ctype)->getName() + "] " + this->querySkillCheck(lineStart, (findKtype(ctype)->name).c_str());
}


//Roll ability checks for all group members based on ability name or abbreviation. Default to Strength.
std::string Group::queryAbilityCheck(const char* lineStart, const char* ability){
    std::string lowName = toLower(ability);

    //Check each ability name and abbreviation for a match
    for (int i = 0;i < 6;i++){
        if (lowName.compare(toLower(abilityAbbr(i))) == 0 || lowName.compare(toLower(abilityName(i))) == 0){
            return this->queryAbilityCheck(lineStart, i);
        }
    }

    return this->queryAbilityCheck(lineStart, 0);
}

//Roll ability checks for all group members
std::string Group::queryAbilityCheck(const char* lineStart, int ability){
    std::string output = "";

    output += abilityName(ability) + " check for " + this->name + "\n";

    //Iterate over group members
    for (LChar l : this->members){
        //Get applicable ability modifier
        int mod = l.scores.getMod(ability);

        //Roll die
        int die = rollDie();
        
        //Format example: "Name (+2): [14] 16"
        output += lineStart + l.name + 
            " (" + (mod >= 0 ? "+" : "") + std::to_string(mod) + "): [" +
            std::to_string(die) + "] " + std::to_string(die + mod) + "\n";
    }
    output.pop_back();

    return output;
}


//Load all groups from the ./Characters directory
int addGroups(){
    //First, see if there is a ./Characters directory
    DIR* dir = opendir("./Characters");

    //If there is not, then stop
    if (dir==NULL){
        printf("No Characters directory found!\n");
        return 1;
    }

    //If there is a ./Characters directory, open its subdirectories
    while(1){
        //Read the next file or directory
        struct dirent* curEntry = readdir(dir);

        //Stop if this is the end of the directory or is a bad file
        if (curEntry==NULL) break;

        //Stop if this is not a directory or begins with '.'
        if (curEntry->d_name[0] == '.') continue;
        std::string path = std::string("./Characters/") + curEntry->d_name;
        DIR* subdir = opendir(path.c_str());
        if (subdir==NULL) continue;
        closedir(subdir);

        //Make a new Group and add it to the groups vector if it has any members
        Group curGroup = Group(curEntry->d_name, false);
        if (curGroup.members.size() > 0) groups.push_back(curGroup);
    }

    closedir(dir);

    //Debug group list
    /*for (Group g : groups){
        printf("%s\n", g.fullDescription().c_str());
    }*/

    return 0;
}

//Reload all groups from the ./Characters directory
int refreshGroups(){
    groups.clear();
    return addGroups();
}

//List all groups in the groups vector
std::string listGroups(){
    std::string output = "";

    for (Group g : groups){
        output += g.name + " (";
        for (std::string a : g.aliases){
            output += a + ", ";
        }
        output.pop_back();
        output.pop_back();
        output += "), ";
    }

    output.pop_back();
    output.pop_back();

    return output;
}

