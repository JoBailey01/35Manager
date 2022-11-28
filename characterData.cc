#include "35Manager.h"

//typedef std::tuple<Skill*, int> skillTuple;
//typedef std::vector<skillTuple> skillSet;

//Determine how many ranks LChar has in a given skill
int LChar::getRanks(Skill* skill){
    //Iterate over skillRanks
    for (skillTuple t : this->skillRanks){
        //Untie t
        int i; Skill* s;
        std::tie(s, i) = t;

        //Is this skill the same as the input skill? If so, return rank count
        if (s == skill) return i;
    }

    //This character has no ranks in this skill
    return 0;
}

//Can this character use this skill?
bool LChar::canUseSkill(Skill* skill){
    //All characters can use untrained skills and high-trainCap skills
    if (!skill->isTrained || skill->trainCap > 0) return true;

    //For trained skills with a TrainCap of 0, characters can only use them if they have ranks
    return (this->getRanks(skill) > 0);
}

//Determine a LChar's skill bonus. Return 0 for an invalid check.
int LChar::getBonus(Skill* skill){
    //Get the applicable ability score modifier and ACP adjustment for this score. Account for NLA.
    int mod = this->scores.getMod(skill->ability) - this->NLA;
    if (skill->isACP) mod -= this->ACP;
    //The Swim skill takes a double ACP penalty
    if (skill->name.compare("Swim") == 0) mod -= this->ACP;

    //Iterate over skillMods
    for (skillTuple t : this->skillMods){
        //Untie t
        int i; Skill* s;
        std::tie(s, i) = t;

        //Is this skill the same as the input skill? If so, return final modifier
        if (s == skill) return i + mod;
    }

    //If we got here, then this character has no ranks in this skill
    //Unrained skill
    if (!skill->isTrained){
        return mod;
    //Trained skill. Return the lower value of either the mod or the trainCap
    } else {
        return skill->trainCap > mod ? mod : skill->trainCap;
    }
}

//Return an human-readable class list based on levelTotals
std::string LChar::getClass(){
    std::string output = "";
    for(std::tuple<CClass*, int> t : this->levelTotals){
        //Extract data from tuple
        int l; CClass* c; std::tie(c, l) = t;
        
        //Add formatted string to output
        output += c->name + " " + std::to_string(l) + ",";
    }

    //Remove final ,
    output.pop_back();

    return output;
}

//List all skill ranks for this character. Input is a string to be added to the front of the line
std::string LChar::getAllRanks(const char* startLine){
    std::string output = "";
    int longestName = 0;
    //Find the longest skill name with ranks
    for (skillTuple t : this->skillRanks){
        //Extract values from tuple
        Skill* s; std::tie(s, std::ignore) = t;
        int len = (int) s->getName().size();

        if (len > longestName) longestName = len;
    }

    //Iterate over all ranks
    for (skillTuple t : this->skillRanks){
        //Extract values from tuple
        int r; Skill* s; std::tie(s, r) = t;
        int len = (int) s->getName().size();

        //Add pre-line and spacing
        output += startLine;
        if (len < longestName){
            for (int i = 0;i < longestName - len;i++){
                output.push_back(' ');
            }
        }

        //Add skill row to output
        output += s->getName() + " | " + std::to_string(r) + "\n";
    }

    //Remove final '\n'
    if (output.size() > 0) output.pop_back();

    return output;
}

//List all modified skill bonuses for this character. Input is a string to be added to the front of the line
std::string LChar::getAllBonuses(const char* startLine){
    std::string output = "";
    int longestName = 0;
    //Find the longest skill name with ranks
    for (skillTuple t : this->skillMods){
        //Extract values from tuple
        Skill* s; std::tie(s, std::ignore) = t;
        int len = (int) s->getName().size();

        if (len > longestName) longestName = len;
    }

    //Iterate over all bonuses
    for (skillTuple t : this->skillMods){
        //Extract values from tuple
        int r; Skill* s; std::tie(s, r) = t;
        int len = (int) s->getName().size();

        //Add pre-line and spacing
        output += startLine;
        if (len < longestName){
            for (int i = 0;i < longestName - len;i++){
                output.push_back(' ');
            }
        }

        //Add skill row to output
        output += s->getName() + " | " + std::to_string(this->getBonus(s)) + "\n";
    }

    //Remove final '\n'
    if (output.size() > 0) output.pop_back();

    return output;
}

//Create a full, human-readable description of this character
std::string LChar::fullDescription(bool addSkillRanks, bool addSkillBonuses){
    std::string output = "";

    output += this->name +
        "\n | " + this->race->name + " " + this->getClass() +
        "\n | " + std::to_string(this->HD) + " HD, ECL " + std::to_string(this->ECL);
        
    if (this->race->LA > 0 ) output += ", LA +" + std::to_string(this->race->LA);
    if (this->NLA > 0) output += ", NLA -" + std::to_string(this->NLA);

    output +=
        "\n | " + std::to_string(this->HP) + " HP" +
        "\n | BAB: +" + std::to_string(this->BAB) +
        "\n | Current ACP: " + std::to_string(this->ACP) +
        "\n | Saves: " + 
            "Fort " + (this->saves.fort < 0 ? "" : "+") + std::to_string(this->saves.fort) +
            ", Ref " + (this->saves.ref < 0 ? "" : "+") + std::to_string(this->saves.ref) +
            ", Will " + (this->saves.will < 0 ? "" : "+") + std::to_string(this->saves.will) +
        "\n | Abilities: Str " + std::to_string(this->scores.scores[STR]) +
            ", Dex " + std::to_string(this->scores.scores[DEX]) +
            ", Con " + std::to_string(this->scores.scores[CON]) +
            ", Int " + std::to_string(this->scores.scores[INT]) +
            ", Wis " + std::to_string(this->scores.scores[WIS]) +
            ", Cha " + std::to_string(this->scores.scores[CHA]);
    
    //Add skill ranks/bonuses, if applicable
    if (addSkillRanks) output += "\n | Skill Ranks:\n" + this->getAllRanks("   | ");
    if (addSkillBonuses) output += "\n | Skill Bonuses:\n" + this->getAllBonuses("   | ");

    //Add notes, if applicable
    if (this->notes.size() > 0){
        output += "\n | ----- Notes -----\n   | ";
        
        //Add notes, with "   | " after every newline or after every MAXLEN characters
        int lineLen = 0;
        for (int i = 0;i < (int) this->notes.size();i++){
            char c = this->notes.at(i);
            if (c=='\n' || lineLen >= MAXLEN){
                output += "\n   | ";
                if (c != '\n') output.push_back(c);
                lineLen = 0;
            }
            else{
                output.push_back(c);
                lineLen++;
            }
        }
    }

    return output;
}

//Create a shortened description of this character (name, class, level, HD, ECL, max HP)
std::string LChar::shortDescription(){
    std::string output = "";
    output += this->name + " (" + this->race->name + " " + this->getClass() + ", ECL " + std::to_string(this->ECL) + ")" +
        " [" + std::to_string(this->HD) + " HD, " + std::to_string(this->HP) +" HP]";

    return output;
}


//Alphabetically sort a skillSet and return it. Used in LChar.update()
skillSet sortSkillSet(skillSet list){
    //Insertion-sort the skillMods list alphabetically. Note that pure parent skills will always end up at the end of the list.
    for (int i = 1;i < (int) list.size();i++){
        //Extract name from skillMods
        skillTuple t = list.at(i);
        Skill* s; std::tie(s, std::ignore) = t;
        std::string iname = s->getName();

        int j = i-1;
        Skill* s2; std::tie(s2, std::ignore) = list.at(j);
        std::string jname = s2->getName();
        while (j >= 0 && jname.compare(iname) > 0){
            //Swap elements at positions j and j+1
            skillTuple temp = list.at(j+1);
            list.at(j+1) = list.at(j);
            list.at(j) = temp;

            //Update jname and j
            j--;
            if (j < 0) break;
            std::tie(s2, std::ignore) = list.at(j);
            jname = s2->getName();
        }
    }

    return list;
}

//Update secondary, calculable details of LChar
int LChar::update(){
    //Calculate HP (see DMG 198) and total up levels
    this->HP = this->HPmod;
    this->levelTotals.clear();
    //Iterate over ordered levels
    for (int i = 0;i < (int) this->levels.size();i++){
        CClass* l = this->levels.at(i);

        //HP at first level
        if (i==0){
            this->HP += l->HD;
        //HP at even levels
        } else if (i % 2 == 0) {
            this->HP += (l->HD/2);
        //HP at higher odd levels
        } else {
            this->HP += (l->HD/2) + 1;
        }
        this->HP += this->scores.getMod(CON);

        //Total up the number of levels in each class

        //Is this class already in levelTotals?
        bool inTotals = false;
        if (this->levelTotals.size() > 0){
            for (int j = 0;j < (int) this->levelTotals.size();j++){
                //Untie tuple
                int total; CClass* c;
                std::tie (c, total) = this->levelTotals.at(j);

                //If the class in the outer for loop is already in levelTotals, increment the total
                if (c == l){
                    total++;
                    this->levelTotals.at(j) = std::tuple<CClass*, int> (l, total);
                    inTotals = true;
                    break;
                }
            }
        }

        //If this class is not in levelTotals already, add it
        if (!inTotals){
            this->levelTotals.emplace_back(std::tuple<CClass*, int> (l, 1));
        }
    }

    //Calculate HD and ECL
    this->HD = (int) this->levels.size();
    this->ECL = this->HD + this->race->LA - this->NLA;

    //Calculate BAB and saves based on levelTotals
    this->BAB = 0;
    this->saves = SavingThrows(); //Default constructor zeroes out all save bonuses
    for (std::tuple<CClass*, int> t : this->levelTotals){
        //Untie tuple
        int i; CClass* c; //i is HD for this class
        std::tie(c, i) = t;

        //Add BAB contribution from this class
        this->BAB = i * c->BAB / 4;

        //Add saving throw contribution from this class
        SavingThrows temp = saveBonuses(c->goodSaves, i);
        this->saves = temp.sumSaves(this->saves);
    }

    //Add appropriate ability score modifiers and NLA to saving throws
    this->saves.fort += this->scores.getMod(CON) - this->NLA + this->plusFort;
    this->saves.ref += this->scores.getMod(DEX) - this->NLA + this->plusRef;
    this->saves.will += this->scores.getMod(WIS) - this->NLA + this->plusWill;

    //Calculate skill modifiers based on ranks and miscellaneous modifiers
    this->skillMods.clear();
    //Add skill ranks
    for (skillTuple s : this->skillRanks){
        this->skillMods.emplace_back(s);
    }
    //Add miscellaneous skill mods, where applicable
    for (skillTuple t : this->skillMisc){
        //Untie t
        int i; Skill* s; //i is miscellaneous modifier
        std::tie(s, i) = t;

        //If this skill has no ranks (and thus is not in skillMods already), add it to skillMods if applicable
        int r = this->getRanks(s); //r is number of ranks for this skill
        if (r == 0){
            //If this skill is untrained, add its modifier to skillMods
            if (!s->isTrained){
                this->skillMods.emplace_back(t);
            }
        //If this skill does have ranks, then add those to the modifier and add that to skillMods
        } else {
            std::tuple<Skill*, int> temp (s, i + r);
            for (int i = 0;i < (int) this->skillMods.size(); i++){
                //Extract the skill pointer from this element
                Skill* sp;
                std::tie(sp, std::ignore) = this->skillMods.at(i);

                //If this is the matching skillTuple, then update this skill tuple with the miscellaneous modifier
                if (sp==s){
                    this->skillMods.at(i) = temp;
                }
            }
        }
        
    }

    //Insertion-sort the skill lists alphabetically. Note that pure parent skills will always end up at the end of the list.
    this->skillRanks = sortSkillSet(this->skillRanks);
    this->skillMods = sortSkillSet(this->skillMods);
    this->skillMods = sortSkillSet(this->skillMods);
    
    //Return 0 for success
    return 0;
}


//Constructor for LChar
LChar::LChar(const char* name, CRace* race, int NLA, int HPmod, int ACP, AScores* scores, std::vector<CClass*> levels, skillSet skillRanks, skillSet skillMisc, int plusFort, int plusRef, int plusWill, std::string notes){
    this->name = std::string(name);
    this->race = race;
    this->NLA = NLA;
    this->HPmod = HPmod;
    this->ACP = ACP;
    this->scores = *scores;
    this->plusFort = plusFort;
    this->plusRef = plusRef;
    this->plusWill = plusWill;
    this->skillRanks = skillRanks;
    this->skillMisc = skillMisc;
    this->notes = notes;

    this->levels = levels;

    //Add RHD to levels
    if (this->race->RHD > 0){
        //Get type class
        CClass* temp = findClass(this->race->name.c_str());

        //Add enough levels to account for the RHD
        for (int i = 0;i < this->race->RHD;i++){
            this->levels.insert(this->levels.begin(), temp);
        }
    }

    this->update();
}

//Blank constructor for LChar. Sets no values.
LChar::LChar(){
    this->name = std::string("[Blank]");
}


//Read n characters from a FILE* pointer and return it as a std::string object
std::string readString(FILE* file, int n){
    std::string output = "";
    char c;
    for (int i = 0;i < n;i++){
        c = fgetc(file);
        if (c==EOF){
            printf(">>%s>\n", output.c_str());
            return output;
        }
        output.push_back(c);
    }
    return output;
}

//Read the next few characters from a file and determine whether it matches a pattern or not
bool readPattern(FILE* file, const char* str){
    int len = strlen(str);
    return (readString(file, len).compare(str) == 0);
}

//readPattern for a single character
bool readPattern(FILE* file, char c){
    char f = fgetc(file);
    return f == c;
}

//Read the next few characters from a file up to the next line break or EOF
std::string readBreak(FILE* file){
    std::string output = "";
    char c = fgetc(file);
    while (c != EOF && c != '\n'){
        output.push_back(c);
        c = fgetc(file);
    }
    return output;
}

//Dummy skillTuple
skillTuple noTuple (findSkill("blank"), 0);

//Read a formatted skill row (up to a '\n' or EOF) and return the appropriate skillTuple. Return noTuple for a bad row.
skillTuple readSkill(FILE* file){
    //Read up to the colon
    std::string skillName = "";
    char c = fgetc(file);
    while (c != ':'){
        //If we hit a line break or EOF, this is a bad row
        if (c == '\n' || c == EOF) return noTuple;
        skillName.push_back(c);
        c = fgetc(file);
    }

    //Get the skill
    Skill* skill = findSkill(skillName.c_str());

    //Get the skill modifier
    std::string modString = "";
    int modSign = 1; //1 or -1

    //Read the modifier after the colon
    c = fgetc(file);
    //Handle a - sign
    if (c == '-'){
        modSign = -1;
    } else if (c == '+'){
        //Do nothing; modSign is already 1
    } else {
        modString.push_back(c);
    }

    c = fgetc(file);
    //Read the rest of the modifier up to the line break or EOF
    while (c != '\n' && c != EOF){
        modString.push_back(c);
        c = fgetc(file);
    }

    //Assemble output
    int mod = (modSign) * (atoi(modString.c_str()));
    return skillTuple (skill, mod);
}


//This is a dummy variable used to store the temporary output of the readCharacter function. It is overwritten after use, so its value must be retrieved from the provided pointer.
LChar tempChar;

//Read in character data from a text file based on its file name. Return a pointer to tempChar on success, or NULL on failure
LChar* readCharacter(const char* filename){
    //Only read files that have the .ddc extension
    const char* ext = filename + (strlen(filename) - 4);
    if(strcmp(ext, ".ddc") != 0) return NULL;

    //Open file pointer to character file
    FILE* sheet = fopen(filename, "r");
    //Stop for bad files
    if (sheet==NULL) return NULL;

    //Expected format is included in the characterFormat.txt file. A blank template is in character.txt
    //Ensure that each line tag (the formatting) is correct during the read process

    //Read character name
    if (!readPattern(sheet, "name:")) return NULL;
    std::string name = readBreak(sheet);

    //Read and process character race name
    if (!readPattern(sheet, "race:")) return NULL;
    CRace* race = findCRace(readBreak(sheet).c_str());

    //Read and process negative level adjustment
    if (!readPattern(sheet, "nla:")) return NULL;
    int NLA = atoi(readBreak(sheet).c_str());

    //Read and process extra HP stat
    if (!readPattern(sheet, "extrahp:")) return NULL;
    int HPmod = atoi(readBreak(sheet).c_str());

    //Read and process extra HP stat
    if (!readPattern(sheet, "acp:")) return NULL;
    int ACP = atoi(readBreak(sheet).c_str());

    //Read levels
    if (!readPattern(sheet, "levels:")) return NULL;

    //Final level vector
    std::vector<CClass*> levels;
    
    //Invalid class pointer for comparison
    CClass* noClass = findClass("invalid class");

    //Iterate over the row
    char c = fgetc(sheet);
    std::string curLevel = "";
    while(1){
        //End of level name and/or line
        if (c == '\n' || c == ','){
            CClass* temp = findClass(curLevel.c_str());
            
            //Add valid classes to the level vector
            if (temp != noClass) levels.emplace_back(temp);
            
            curLevel.erase();

            if (c == '\n') break; //Stop at a line break
        } else {
            //Add all other characters to curLevel
            curLevel.push_back(c);
        }

        //Read next character in line
        c = fgetc(sheet);
    }


    //Read line break
    if (!readPattern(sheet, '\n')) return NULL;

    //Read and process ability scores
    if (!readPattern(sheet, "STR:")) return NULL;
    int str1 = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "DEX:")) return NULL;
    int dex1 = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "CON:")) return NULL;
    int con1 = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "INT:")) return NULL;
    int int1 = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "WIS:")) return NULL;
    int wis1 = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "CHA:")) return NULL;
    int cha1 = atoi(readBreak(sheet).c_str());
    
    AScores scores = AScores(str1,dex1,con1,int1,wis1,cha1);

    //Read line break
    if (!readPattern(sheet, '\n')) return NULL;

    //Read and process extra skill modifiers
    if (!readPattern(sheet, "plusfort:")) return NULL;
    int plusfort = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "plusref:")) return NULL;
    int plusref = atoi(readBreak(sheet).c_str());
    if (!readPattern(sheet, "pluswill:")) return NULL;
    int pluswill = atoi(readBreak(sheet).c_str());

    //Read line break
    if (!readPattern(sheet, '\n')) return NULL;

    //Get the noSkill pointer for later comparison
    Skill* noSkill = findSkill("blank skill name");

    //Read skill ranks
    skillSet ranks = skillSet();
    if (!readPattern(sheet, "ranks:\n")) return NULL;
    //Read each row, one by one
    while(1){
        //Every skill row begins with four spaces. If there is no tab, we've reached the end of the skills
        if(readPattern(sheet, ' ')){
            if (!readPattern(sheet, ' ')) break;
            if (!readPattern(sheet, ' ')) break;
            if (!readPattern(sheet, ' ')) break;
        } else {
            break;
        }

        //Read the skill row
        skillTuple thisSkill = readSkill(sheet);
        //Extract skill pointer
        Skill* s; std::tie(s, std::ignore) = thisSkill;

        //If this is a bad skill, then ignore it
        if (s==noSkill) continue;

        //Otherwise, add it to the ranks list
        ranks.emplace_back(thisSkill);
    }
    
    
    //Read miscellaneous skill modifiers (the 'm' has been absorbed)
    if (!readPattern(sheet, "odifiers:\n")) return NULL;
    skillSet miscMods = skillSet();
    //Read each row, one by one
    while(1){
        //Every skill row begins with four spaces. If there is no tab, we've reached the end of the skills
        if(readPattern(sheet, ' ')){
            if (!readPattern(sheet, ' ')) break;
            if (!readPattern(sheet, ' ')) break;
            if (!readPattern(sheet, ' ')) break;
        } else {
            break;
        }

        //Read the skill row
        skillTuple thisSkill = readSkill(sheet);
        //Extract skill pointer
        Skill* s; std::tie(s, std::ignore) = thisSkill;

        //If this is a bad skill, then ignore it
        if (s==noSkill) continue;

        //Otherwise, add it to the ranks list
        miscMods.emplace_back(thisSkill);
    }

    //Read in the general notes header ("notes:\n" but the 'n' has been absorbed already)
    if (!readPattern(sheet, "otes:\n")) return NULL;
    std::string notes = "";
    c = fgetc(sheet);
    while (1){
        //Stop at the end of the file
        if (c == EOF) break;

        //Otherwise, add this character to notes
        notes.push_back(c);
        c = fgetc(sheet);
    }
    
    //This copy-pasted code is annoying but necessary

    fclose(sheet);

    //The sheet is finished. Fill tempChar with its data and return the pointer
    tempChar = LChar(name.c_str(), race, NLA, HPmod, ACP, &scores, levels, ranks, miscMods, plusfort, plusref, pluswill, notes);

    return &tempChar;
}

