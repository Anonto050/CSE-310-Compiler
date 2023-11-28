#include<bits/stdc++.h>

using namespace std;

class symbolInfo
{

    string name;
    string type;
    symbolInfo* next;

public :

    symbolInfo()
    {
    }

    symbolInfo(string name, string type)
    {
        this->name = name;
        this->type = type;
    }

    symbolInfo(string name,string type,symbolInfo* next)
    {
        this->name = name;
        this->type = type;
        this->next = next;
    }

    void setName(string name)
    {
        this->name = name;
    }

    void setType(string type)
    {
        this->type = type;
    }

    void setNext(symbolInfo* next)
    {
        this->next = next;
    }

    string getName()
    {
        return name;
    }

    string getType()
    {
        return type;
    }

    symbolInfo* getNext()
    {
        return next;
    }

    friend ostream& operator<<(ostream& out, symbolInfo& symbol)
    {
        out << "<" << symbol.name << "," << symbol.type << ">";
        return out;
    }
};

class scopeTable
{
    int uniq_id;
    int position;
    int bucket_number;
    int totNum_buckets;

    symbolInfo** symbol_list;
    scopeTable* parent_scope;

public :

    scopeTable() {}

    scopeTable(int uniq_id, int totNum_buckets, scopeTable* parent_scope)
    {
        this->uniq_id = uniq_id;
        this->totNum_buckets = totNum_buckets;
        this->parent_scope = parent_scope;

        symbol_list = new symbolInfo*[totNum_buckets];

        for(int i=0; i<totNum_buckets; i++)
        {
            symbol_list[i] = NULL;
        }

    }

    unsigned long long int hash_function(string name)
    {

        unsigned long long int hash = 0;
        unsigned long long int i = 0;
        unsigned long long int len = name.length();

        for (i = 0; i < len; i++)
        {
            hash = ((name[i]) + (hash << 6) + (hash << 16) - hash);
        }

        return hash%totNum_buckets; 
    }


    void setUniq_id(int uniq_id)
    {
        this->uniq_id = uniq_id;
    }

    void setNum_buckets(int totNum_buckets)
    {
        this->totNum_buckets = totNum_buckets;
    }

    void setParent_scope(scopeTable* parent_scope)
    {
        this->parent_scope = parent_scope;
    }

    scopeTable* getParent_scope()
    {
        return parent_scope;
    }

    int getUniq_id()
    {
        return uniq_id;
    }

    int getNum_buckets()
    {
        return totNum_buckets;
    }

    symbolInfo* look_up(string name, ofstream& outfile, int check = 1)
    {

        int bucket_number = hash_function(name);

        symbolInfo* temp = symbol_list[bucket_number];
        position = 1;


        while(temp != NULL)
        {
            if(temp->getName() == name)
            {
                return temp;
            }
            position++;
            temp = temp->getNext();
        }

        return NULL;

    }

    bool Insert(symbolInfo& symbol, ofstream& outfile)
    {
        if(look_up(symbol.getName(),outfile,0)!= NULL)
        {
            
           return false;
        }

        int bucket_number = hash_function(symbol.getName());

        position = 1;

        symbolInfo* temp = symbol_list[bucket_number];

        if(temp == NULL)
        {
            symbol_list[bucket_number] = &symbol;
            symbol.setNext(NULL);

            
            return true;
        }

        while(temp->getNext()!= NULL)
        {
            temp = temp->getNext();
            position++;
        }

        temp->setNext(&symbol);
        symbol.setNext(NULL);
        position++;

        
        return true;
    }

    bool Delete(string name, ofstream& outfile)
    {
        if(look_up(name,outfile,0) == NULL)
        {
            
            return false;
        }

        int bucket_number = hash_function(name);

        position = 1;

        symbolInfo* temp = symbol_list[bucket_number];

        if(temp->getName() == name)
        {
            temp = temp->getNext();
            symbol_list[bucket_number] = temp;
            
            return true;
        }

        while(temp->getNext()!= NULL)
        {
            if(temp->getNext()->getName() == name )
            {
                temp->setNext(temp->getNext()->getNext());
                
                return true;
            }
            position++;
            temp = temp->getNext();
        }

    }

    void print(ofstream& outfile)
    {
        //cout << "\t\t" << "ScopeTable# " << uniq_id << endl;
        outfile << "\t" << "ScopeTable# " << uniq_id << endl;

        for(int i = 0; i < totNum_buckets; i++)
        {
            int j = i+1;
        

            symbolInfo* temp_symbol = symbol_list[i];
            if(temp_symbol != NULL){
                outfile << "\t" << j<< "--> ";
                j = 0;
            }

            while(temp_symbol != NULL)
            {
                //cout<< "\t\t" << j << "--> "<< *temp_symbol<<endl;
                outfile << *temp_symbol<<" ";

                temp_symbol = temp_symbol->getNext();
            }
            if(j==0){
                outfile<<endl;
            }
        }
    }

    ~scopeTable()
    {
        delete[] symbol_list;
    }
};

class symbolTable
{
    scopeTable* curr_scope;
    scopeTable* temp;

public:
    symbolTable()
    {
        curr_scope = NULL;
    }

    symbolTable(int uniq_id, int totNum_buckets,ofstream& outfile)
    {
        curr_scope = NULL;
        temp = new scopeTable(uniq_id, totNum_buckets, curr_scope);

        curr_scope = temp;
    }

    ~symbolTable()
    {

    }

    void enter_scope(int uniq_id,int totNum_buckets,ofstream& outfile)
    {
        temp = new scopeTable(uniq_id, totNum_buckets, curr_scope);

        curr_scope = temp;

    }

    void exit_scope(ofstream& outfile, int r = 0)
    {
        //cout<<(r==0)<<endl;
        int t = curr_scope->getUniq_id();

        if(t == 1 && r == 0)
        {
            /* outfile<<"\t"<<"ScopeTable# 1 cannot be removed"<<endl;
            cout<<"\t"<<"ScopeTable# 1 cannot be removed"<<endl; */
            return;
        }
        /* cout<<"\t"<<"ScopeTable# "<<curr_scope->getUniq_id()<<" removed"<<endl;
        outfile<<"\t"<<"ScopeTable# "<<curr_scope->getUniq_id()<<" removed"<<endl; */

        curr_scope = curr_scope->getParent_scope();

        return;
    }

    symbolInfo* LookUp(string name, ofstream& outfile)
    {
        symbolInfo* temp = NULL;

        scopeTable* current = curr_scope;

        while(current != NULL)
        {
            temp = current->look_up(name, outfile);

            if(temp != NULL)
            {
                break;
            }
            current = current->getParent_scope();
            //cout<<current<<endl;
        }

        return temp;
    }

    bool Insert(symbolInfo& symbol, ofstream& outfile)
    {
        return curr_scope->Insert(symbol, outfile);
    }

    bool Remove(string name, ofstream& outfile)
    {
        if(curr_scope == NULL)
        {
            

            return false;
        }

        return curr_scope->Delete(name, outfile);
    }

    void print_current(ofstream& outfile)
    {
        if(curr_scope == NULL)
        {
            return ;
        }

        curr_scope->print(outfile);
        return ;
    }

    void print_all(ofstream& outfile)
    {
        scopeTable* temp = curr_scope;

        while(temp != NULL)
        {
            temp->print(outfile);
            temp = temp->getParent_scope();
        }
    }

    scopeTable* get_current()
    {
        return curr_scope;
    }

};

/* int main()
{

   ifstream infile;
    ofstream outfile; 

    //char const* out = "output.txt";

    infile.open("test_input.txt");
    outfile.open("output.txt");

    string operation, name, type;
    int bucket_numbers, id, remain, cmp, line_num;

    infile>>bucket_numbers;

    id = 1;
    remain = 1;
    cmp = 0;
    line_num = 0;
//isstresm
//getline
// L I J
//name type
    symbolTable st;
    //st.get_current()->setParent_scope(NULL);

    st.enter_scope(id,bucket_numbers,outfile);

    string line;

    while(getline(infile,line))
    {
        stringstream linestream(line);
        int cnt = 0;
        string item;

        if(line_num != 0){
            cmp++;
            cout<<"Cmd "<<cmp<<":";
            outfile<<"Cmd "<<cmp<<":";
        }

        line_num++;

        while (getline(linestream, item, ' '))
        {
            cout<<" "<<item;
            outfile<<" "<<item;
            //cout << item << endl;
            if(cnt == 0 && item != " ")
            {
                operation = item;
            }
            else if(cnt == 1 && item != " ")
            {
                name = item;
            }
            else if(cnt == 2 && item != " ")
            {
                type = item;
            }
            cnt++;
            //cout<<item<<" "<<cnt<<endl;
        }
        if(line_num>1){
        cout<<endl;
        outfile<<endl;
        }

        if(operation == "I")
        {
            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<" "<<type<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<" "<<type<<endl;

            if(cnt != 3)
            {
                cout<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
                outfile<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
            }
            else
            {
                symbolInfo* symbol = new symbolInfo(name, type);

                st.Insert(*symbol, outfile);
            }

        }

        else if(operation == "L")
        {

            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<endl;

            if(cnt != 2)
            {
                cout<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
                outfile<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
            }
            else
            {
                symbolInfo* sbl = st.LookUp(name,outfile);
                if(sbl == NULL)
                {
                    cout<<"\t"<<"'"<<name<<"' not found in any of the ScopeTables"<<endl;
                    outfile<<"\t"<<"'"<<name<<"' not found in any of the ScopeTables"<<endl;
                }
            }
        }
        else if(operation == "D")
        {
            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<endl;

            if(cnt != 2)
            {
                cout<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
                outfile<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
            }
            else
            {
                st.Remove(name,outfile);
            }

        }
        else if(operation == "P")
        {
            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<" "<<name<<endl;

            if(cnt != 2)
            {
                cout<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
                outfile<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
            }
            else
            {
                if(name == "C")
                {
                    st.print_current(outfile);
                }
                else if(name == "A")
                {
                    st.print_all(outfile);
                }
            }
        }
        else if(operation == "S")
        {
            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<endl;

            if(cnt != 1)
            {
                cout<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
                outfile<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
            }

            else
            {
                id++;
                st.enter_scope(id, bucket_numbers, outfile);

                remain++;
            }
        }
        else if(operation == "E")
        {
            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<endl;

            if(cnt != 1)
            {
                cout<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
                outfile<<"\t"<<"Number of parameters mismatch for the command "<<operation<<endl;
            }
            else
            {
                if(remain>1){
                    st.exit_scope(outfile);
                    remain--;
                }
                else{
                    outfile<<"\t"<<"ScopeTable# 1 cannot be removed"<<endl;
                    cout<<"\t"<<"ScopeTable# 1 cannot be removed"<<endl;
                }
            }
        }
        else if(operation == "Q")
        {
            //cmp++;
            //cout<<"Cmd "<<cmp<<": "<<operation<<endl;
            //outfile<<"Cmd "<<cmp<<": "<<operation<<endl;
            break;
        }
    }

    int j = 0;
    while(j<remain){
        st.exit_scope(outfile,1);
        j++;
    }

    infile.close();
    outfile.close();

    return 0;
}
 */