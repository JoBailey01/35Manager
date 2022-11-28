#include "35Manager.h"
#include <ctime>

#include <readline/readline.h>
#include <readline/history.h>

//Display usage text/guide
void usage(){
    const char* guide = 
    "Usage:\n"
    "   allg                                | List all groups by name and alias\n"
    "   reset                               | Reload all group data from files\n"
    "   exit                                | Close 35Manager\n"
    "   alls                                | List all skills\n"
    "   sd                                  | See full description of one skill\n"
    "   allt                                | List all creature types by knowledge skill\n"
    "   sg [group name or alias]            | Set active group\n"
    "   ag                                  | See name of active group\n"
    "   names                               | List members of active group\n"
    "   sdesc                               | See short description of active group\n"
    "   fdesc                               | See full description of active group\n"
    "   ac [ability name or abbreviation]   | Make ability check for active group\n"
    "   sc [skill name or alias]            | Make skill check for active group\n"
    "   tk [creature type]                  | Make Knowledge check for a creature type for active group\n"
    "   st [saving throw name]              | Make saving throw for active group\n"
    "   log [log entry]                     | Add timestamped entry to active group's log\n"
    "   logs                                | View log for active group\n"
    "";

    printf("%s", guide);
}


//Current group
Group* curGroup = NULL;

//Set start-of-line characters for parseCommand
const char* sl = " | ";

//Set start of log entry format
const char* logStart = "DM's log,";

//Set start-of-line prompt
const char* prompt = "> ";

//Parse a command from the terminal. Return 0 for a good command, 1 for a bad command, 2 for exit.
int parseCommand(const char* input){
    //Stop if this command is blank
    if (strlen(input) <= 0) return 1;

    //Find the command identifier (the first word in the command)
    std::string start = "";
    
    const char* c = input;
    while(*c != '\0' && *c != ' ' && *c != '\n'){
        start.push_back(*c);
        c++;
    }

    //Remove final space, if necessary

    //Find the parametres
    std::string params = "";
    if (*c != '\0') c++;
    while(*c != '\0' && *c != '\n'){
        params.push_back(*c);
        c++;
    }

    //Parse commands

    //List all groups
    if(start.compare("allg")==0){
        if (params.size() > 0) return 1;
        printf(" + Groups: %s\n", listGroups().c_str());
        return 0;
    
    //Read group data from file
    } else if(start.compare("reset")==0){
        if (params.size() > 0) return 1;
        refreshGroups();
        printf(" + Refreshed group data\n");
        return 0;

    //Set active group
    } else if(start.compare("sg")==0){
        if (params.size() <= 0) return 1;
        Group* temp = findGroup(params.c_str());
        if (temp != NULL){
            curGroup = temp;
            printf(" + Set active group to %s.\n", curGroup->name.c_str());
        } else {
            printf(" + Group '%s' does not exist.\n", params.c_str());
        }
        return 0;
    } else if (start.compare("exit") == 0){
        if (params.size() > 0) return 1;
        printf("Goodbye!\n");
        return 2;
    } else if (start.compare("alls") == 0){
        if (params.size() > 0) return 1;
        printf(" + Skills list:\n%s\n", listSkills(true).c_str());
        return 0;
    } else if (start.compare("sd") == 0){
        if (params.size() <= 0) return 1;
        std::string detString = "";
        Skill* s = findSkill(params.c_str());
        detString += s->longString() + s->detailString('\t');
        printf(" + %s\n", detString.c_str());
        return 0;
    } else if (start.compare("allt") == 0){
        if (params.size() > 0) return 1;
        printf(" + Knowledge skills and creature types:\n%s\n", listKnowledge().c_str());
        return 0;
    }

    //The remaining commands require an active group
    bool hasGroup = true;
    if (curGroup == NULL){
        hasGroup = false;
    }

    //See name of active group
    if(start.compare("ag")==0 && hasGroup){
        if (params.size() > 0) return 1;
        printf(" + Active group: %s\n", curGroup->name.c_str());
        return 0;

    //List group members' names
    } else if(start.compare("names")==0 && hasGroup){
        if (params.size() > 0) return 1;
        printf(" + Group names:\n%s\n", curGroup->queryNames(sl).c_str());
        return 0;

    //Give short description of group
    } else if(start.compare("sdesc")==0 && hasGroup){
        if (params.size() > 0) return 1;
        printf(" + Short description:\n%s\n", curGroup->queryShortDescription(sl).c_str());
        return 0;
    
    //Give full description of group
    } else if(start.compare("fdesc")==0 && hasGroup){
        if (params.size() > 0) return 1;
        printf(" + Full description:\n%s\n", curGroup->fullDescription(true, false).c_str());
        return 0;

    //Make ability check for active group
    } else if(start.compare("ac")==0 && hasGroup){
        if (params.size() <= 0) return 1;
        printf(" + %s\n", curGroup->queryAbilityCheck(sl, params.c_str()).c_str());
        return 0;
    
    //Make skill check for active group
    } else if(start.compare("sc")==0 && hasGroup){
        if (params.size() <= 0) return 1;
        printf(" + %s\n", curGroup->querySkillCheck(sl, params.c_str()).c_str());
        return 0;
    
    //Make type-knowledge check for active group
    } else if(start.compare("tk")==0 && hasGroup){
        if (params.size() <= 0) return 1;
        printf(" + %s\n", curGroup->queryCreatureKnowledge(sl, params.c_str()).c_str());
        return 0;
    
    //Make saving throw for active group
    } else if(start.compare("st")==0 && hasGroup){
        if (params.size() <= 0) return 1;
        printf(" + %s\n", curGroup->querySaves(sl, params.c_str()).c_str());
        return 0;
    
    //Add a log entry to notes
    } else if(start.compare("log")==0 && hasGroup){
        if (params.size() <= 0) return 1;

        //Format params
        int lineLen = 0;
        std::string formatParams = "";
        for (int i = 0;i < (int) params.size();i++){
            char pc = params.at(i);
            if (pc=='\n' || lineLen >= MAXLEN){
                formatParams += std::string("\n") + sl;
                if (pc != '\n') formatParams.push_back(pc);
                lineLen = 0;
            }
            else{
                formatParams.push_back(pc);
                lineLen++;
            }
        }

        //Get current date and time
        time_t now = time(0);
        tm* ft = gmtime(&now);
        
        std::string date = std::string(ft->tm_hour < 10 ? "0" : "") + std::to_string(ft->tm_hour) + ":" +
            (ft->tm_min < 10 ? "0" : "") + std::to_string(ft->tm_min) + ":" +
            (ft->tm_sec < 10 ? "0" : "") + std::to_string(ft->tm_sec) + " UTC, " +
            (ft->tm_mday < 10 ? "0" : "")  + std::to_string(ft->tm_mday) + "/" +
            (ft->tm_mon < 10 ? "0" : "") + std::to_string(ft->tm_mon) + "/" +
            std::to_string(ft->tm_year + 1900);

        std::string logEntry = std::string() + "" + logStart + " " + date + "\n" + sl +
            formatParams + "\n\n";
        
        //Update notes document and local variable
        FILE* logFile = fopen(curGroup->logPath.c_str(), "a+");
        
        fprintf(logFile, "%s", logEntry.c_str());

        fclose(logFile);

        printf(" + Log entry added\n");
        return 0;

    //View notes and synchronise with notes document
    } else if(start.compare("logs")==0 && hasGroup){
        if (params.size() > 0) return 1;
        FILE* logFile = fopen(curGroup->logPath.c_str(), "a+");
        
        std::string logs = "";
        char nc = fgetc(logFile);
        while (nc != EOF){
            logs.push_back(nc);
            nc = fgetc(logFile);
        }

        fclose(logFile);

        printf(" + Logs:\n%s\n", logs.c_str());
        return 0;

    
    //Invalid commands
    } else {
        //Error messages
        if (hasGroup){
            printf(" + Invalid command\n");
        } else {
            printf(" + No active group selected\n");
        }
        return 1;
    }
    return 1;
}

//Run the terminal
void terminal(){
    /*parseCommand("sg testers");
    parseCommand("sdesc");
    parseCommand("log test1");
    parseCommand("logs");
    usage();*/

    //Startup
    printf("Welcome to 35Manager!\n");
    //usage();

    //Start terminal loop
    while(1){
        //Start-of-line prompt
        printf("%s", prompt);

        //Store line values
        char* line = NULL;
        size_t len = 0;

        //Get line from terminal
        getline(&line, &len, stdin);

        //char* line = readline(prompt);

        //Run command and get return value
        int ret = 1;
        if (line != NULL) ret = parseCommand(line);

        //line has been allocated, so we must free it
        free(line);

        //Handle non-0 return values
        if (ret==1) usage();
        else if (ret==2) return;
    }
}

