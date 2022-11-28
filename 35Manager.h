//Header stuff
#define MANAGER_HEADER

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <stdarg.h>

#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

//Check a die size for validity. Default to 8 if it is invalid. Return the die size.
int checkDieSize(int);

//Check a BAB progression (in units of 1/4HD) for validity. Must be 2, 3, or 4
int checkBAB(int);
std::string getBABProgression(int);

//The six ability scores
enum abilities{STR=0, DEX=1, CON=2, INT=3, WIS=4, CHA=5};

//Get the full name of an ability
std::string abilityName(int);
//Get the abbreviation of an ability
std::string abilityAbbr(int);

//Sourcebooks
enum sourcebooks{PHB=0, EPH=1, MM=2, ROD=3};
std::string bookAbbr(int);
std::string bookName(int);

//Make lowercase strings
std::string toLower(const char*);
std::string toLower(std::string);

/**
 * @brief This class stores a single skill from the skill list and all relevant information about it
 * 
 * This class' methods, including its constructor, are expanded in 35Manager.cc
 */
class Skill {
    public:
        //Formal name of the skill
        std::string name;

        //The ability score for this skill. Defaults to INT (3) if invalid input is provided.
        int ability;

        //Is this skill a child skill (e.g. Knowledge(Arcana))?
        bool isChild;

        //Is this a trained skill?
        bool isTrained;

        //Does this skill have a cap for untrained use? Defaults to 0 for a trained skill.
        int trainCap;
        
        //Does this skill respect the ACP (Armour Check Penalty)?
        bool isACP;

        //Is this skill a parent skill (e.g., Knowledge)?
        bool isParent;
        
        //Index of this skill's parent skill. -1 if this skill has no parent.
        int parent;

        //Can this skill be tried again? Defaults to 0 (Yes)
        int tryAgain;

        //Does the DM roll this check instead of the player? true even if players sometimes roll the check.
        bool DMRoll;

        //What sourcebook contains this skill (and on what page?)
        int sourcebook;
        int page;

        //Aliases for use in processing user input
        std::vector<std::string> alias;
    
    //Get this skill's full name, including its parent name if applicable
    std::string getName();

    //Return string of skill
    std::string longString();

    //Return detailed breakdown of skill's details. Argument is a character, like '\t', that is added to the start of each line
    std::string detailString(char);

    //Return comma-separated list of aliases
    std::string getAliases();
    
    //Constructor
    Skill(const char* name, int ability, bool isTrained, int trainCap, bool isACP, bool isParent, int parent,
        int tryAgain, bool DMRoll, int sourcebook, int page, std::vector<std::string> aliases);
    
    //Get a current pointer to the parent object of a child skill, or noSkill if the skill object is not a child
    Skill* getParent();
};

//Create a blank skill
Skill blankSkill();

//Retrieve a skill from the skills vector based on its name or alias. Return default Skill on a failure
Skill* findSkill(const char*);

//Add the list of skills to the skills vector. Returns 0 for a success.
int addSkillList();

//Create a list of all the skills, with or without their alias lists
std::string listSkills(bool);


//Enum for tracking what saves are Good for a class (or creature type). N is no good save, although any value <1 >7 will do.
enum goodSaveNum{N=0, F=1, R=2, W=3,
    FR=4, RF=4,
    FW=5, WF=5,
    RW=6, WR=6,
    FRW=7, FWR=7, RFW=7, RWF=7, WFR=7, WRF=7};

/**
 * @brief Stores three throw bonuses: fort, ref, will
 * 
 */
class SavingThrows {
    public:
        int fort;
        int ref;
        int will;

        //Blank constructor (defaults to 0 for all scores)
        SavingThrows();

        //Constructor
        SavingThrows(int fort, int ref, int will);
        
        //Add two SavingThrows objects
        SavingThrows sumSaves(SavingThrows);
};

//Convert a goodSaveNum back into a list of characters (FRW)
std::string saveLetters(int);

//Is a given saving throw good for this goodSaveNum value?
bool goodSave(int, int);

//Get the saving throw bonus for all three saving throws, in order (FRW), based on HD and good saves
SavingThrows saveBonuses(int, int);


/**
 * @brief This class stores a creature type with its name, HD size, general attributes, and description
 * This class also handles creature subtypes
 * 
 */
class CreatureType {
    public:
        //Name of the creature type
        std::string name;

        //Hit die size. Defaults to d8
        int HD;

        //BAB progression (in quarters of HD)
        int BAB;

        //General description of this creature type (optional)
        std::string description;
        
        //Good saving throws (irrelevant for subtypes)
        int goodSaves;

        //Sourcebook and page
        int sourcebook;
        int page;
        
        //Is this creature type a subtype?
        bool isSubtype;

        //Get this creature type's name
        std::string getName();

        //Get full extended description of this creature type
        std::string fullDescription();

        //Basic constructor
        CreatureType(const char* name, int HD, int BAB, const char* description, int goodSaves, int sourcebook, int page, bool isSubtype);
};


//Add a list of creature types to the types vector. Returns a pointer to the types vector, or NULL for failure
std::vector<CreatureType>* addCtypes();

/**
 * @brief This class stores a pointer to a Knowledge skill (AFTER the skills list is fully populated) and its associated creature types
 *
 */
class TypeKnowledge {
    public:
        //The Knowledge skill associated with this object
        Skill* knowledgeSkill;

        //A list of the creature types associated with this skill, stored as pointers
        std::vector<CreatureType> types;

        //Get a string form of the creature types associated with this TypeKnowledge
        std::string getCreatureTypes();

        //Constructor
        TypeKnowledge(Skill* knowledgeSkill, std::vector<CreatureType> types);
};

//Find a TypeKnowledge object based on its name or alias
TypeKnowledge* findKnowledge(const char*);

//Find a creature type's Knowledge skill object based on its name
Skill* findKtype(const char*);

//Find a creature type object based on its name
CreatureType* findCreatureType(const char*);

//List all Knowledge skills and their creature types
std::string listKnowledge();

//Add a list of knowledge-type associations to the ktypes vector. Returns 0 for success.
int addKtypes();


/**
 * @brief This class stores a race option, with its level adjustment, type, subtype(s), and RHD (where applicable)
 * CRace stands for 'Character Race'
 * NOTE: CRace does not store language or bonus languages
 */
class CRace {
    public:
        //Name
        std::string name;
        
        //Level adjustment
        int LA;

        //Type and subtype(s)
        CreatureType* type;
        std::vector<CreatureType> subtypes;

        //RHD (usually 0)
        int RHD;

        //Sourcebook and page
        int sourcebook;
        int page;

        //Constructor
        CRace(const char* name, int LA, CreatureType* type, std::vector<CreatureType> subtypes, int RHD, int sourcebook, int page);

        //Get the full description of this race
        std::string fullDescription();
};

//Find a character race by name
CRace* findCRace(const char*);

//List all race options
std::string listRaces();

//Add the list of race options to the CRaces vector. Returns 0 for success.
int addCRaces();

/**
 * @brief This class stores a playable class (which can be a creature type)
 * 
 */
class CClass {
    public:
        //Is this class also a creature type?
        bool isType;

        //Name
        std::string name;

        //Saving throws
        int goodSaves;

        //Hit die size
        int HD;

        //BAB progression (2, 3, or 4)
        int BAB;

        //Sourcebook reference
        int sourcebook;
        int page;

        //Full class description
        std::string fullDescription();

        //Constructor
        CClass(bool isType, const char* name, int goodSaves, int HD, int BAB, int sourcebook, int page);
};

//Find a class by name
CClass* findClass(const char*);

//List all class options, possibly including creature type classes
std::string listClasses(bool);

//Add the list of CClass options (including creature types) to the CClasses vector. Returns 0 for success.
int addCClasses(std::vector<CreatureType>);

/**
 * @brief Class to store ability scores
 * AScores is short for Ability Scores
 */
class AScores {
    public:
        //Ability scores
        int scores[6];

        //Get the modifier for an ability score, or 0 if the score requested is invalid
        int getMod(int);

        //Blank constructor (defaults to 10 for all scores)
        AScores();
        //Constructor
        AScores(int strength, int dexterity, int constitution, int intelligence, int wisdom, int charisma);
};

//Typedefs for skill-rank and skill-modifier tuples in character data
typedef std::tuple<Skill*, int> skillTuple;
typedef std::vector<skillTuple> skillSet;

//The maximum length of printed text lines in arbitrary fields like character/group notes
#define MAXLEN 150

/**
 * @brief This class stores a character, PC or NPC
 * 'LChar' is short for 'Levelled Character', used to distinguish this class from 'character', as in a char
 */
class LChar {
    public:
        //Character name
        std::string name;

        //Race (CRace) pointer
        CRace* race;

        //Negative level adjustment (usually 0), stored as a positive number
        //Player's Guide to Faerun, page 190
        //-1 penalty on skill checks, attack rolls, saving throws, level checks, racial AC, and racial LA per NLA
        int NLA;

        //Extra HP from feats, etc.
        int HPmod;

        //Current ACP
        int ACP;

        //Ability scores
        AScores scores;

        //Character class levels, in order
        std::vector<CClass*> levels;

        //Character class level count per class
        std::vector<std::tuple<CClass*, int>> levelTotals;

        //Skill ranks
        skillSet skillRanks;

        //Miscellaneous skill modifiers
        skillSet skillMisc;

        //Miscellaneous saving throw modifiers
        int plusFort; int plusRef; int plusWill;

        //General character notes
        std::string notes;

        //Saving throws, HD, HP total, ECL, and overall skill modifier can be calculated using the above information
        int HP;
        int HD;
        int BAB;
        SavingThrows saves;
        int ECL;
        skillSet skillMods; //Skill modifiers, EXCLUDING ability score modifier

        //Calculate secondary details (HP, HD, saves, ECL, skill modifiers) based on primary details (race, levels, skill ranks, miscellaneous skill modifiers). Return 0 for success.
        int update();

        //Get the skill modifier for this character based on a pointer to a Skill* object
        int getBonus(Skill*);

        //Get ranks for this skill
        int getRanks(Skill*);

        //Can this character use this skill (i.e., if the skill is trained, does LChar have ranks?)
        bool canUseSkill(Skill*);

        //Display character levels from levelTotals
        std::string getClass();

        //Display list of skill ranks
        std::string getAllRanks(const char*);

        //Display list of (modified) skill bonuses
        std::string getAllBonuses(const char*);

        //Display a full description for this character, with or without skill ranks, with or without skill bonuses
        std::string fullDescription(bool, bool);

        //Display a short description for this character (name, class, level, HD, ECL, max HP)
        std::string shortDescription();

        //Main constructor
        LChar(const char* name, CRace* race, int NLA, int HPmod, int ACP, AScores* scores, std::vector<CClass*> levels, skillSet skillRanks, skillSet skillMisc, int plusFort, int plusRef, int plusWill, std::string notes);
        //Blank constructor for LChar
        LChar();
};

//Create a new LChar object by reading in a file
LChar* readCharacter(const char*);

/**
 * @brief Roll a die of the given size. If no input is given, roll a d20
 * 
 */
int rollDie(int);
int rollDie();

/**
 * @brief This class stores a named group of characters, each of which is represented by a LChar object
 * This is especially useful for storing adventuring parties
 */
class Group {
    public:
        //Group name
        std::string name;

        //Group's aliases
        std::vector<std::string> aliases;

        //Notes on this group, stored in the notes.txt file
        std::string notes;

        //The file address of this log document
        std::string logPath;

        //Is this group the main party of PCs?
        bool isParty;

        //All the members of the party
        std::vector<LChar> members;

        //Print out the full descriptions of all the members of a group, with or without skill ranks, with or without skill bonuses
        std::string fullDescription(bool, bool);
        std::string fullDescription();
        
        //Specific group queries. Each returns a result for every applicable member of a group. Most queries can accept a start-of-line formatting value.
        std::string queryNames(const char*);
        std::string queryShortDescription(const char*);
        std::string queryLongDescription(bool, bool);
        
        std::string querySaves(const char*, const char*);
        std::string querySaves(const char*, int);
        
        std::string querySkillCheck(const char*, const char*);
        std::string queryCreatureKnowledge(const char*, const char*);
        
        std::string queryAbilityCheck(const char*, const char*);
        std::string queryAbilityCheck(const char*, int);

        //Constructor to read a group as files from a subdirectory. Always starts in the the ./Characters directory
        Group(const char* directory, bool isParty);
};

//Load all groups from the ./Characters directory
int addGroups();

//Re-read the input files for all groups
int refreshGroups();

//Display list of all groups
std::string listGroups();

//Find a specific group based on its alias, or NULL if no such group exists
Group* findGroup(const char*);

//Run the terminal interface
void terminal();

//#endif //ifdef MANAGER_HEADER
