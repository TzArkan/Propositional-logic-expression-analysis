#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <map>

// Operatii logice
int impl(int a, int b) {
    if ((a == 0) || (b == 1)) return 1;
    return 0;
}

int disj(int a, int b) {
    if ((a == 0) && (b == 0)) return 0;
    return 1;
}

int conj(int a, int b) {
    if ((a == 1) && (b == 1)) return 1;
    return 0;
}

int echiv(int a, int b) {
    if (a == b) return 1;
    return 0;
}

int neg(int a) {
    if (a == 0) return 1;
    return 0;
}

// Verifica operatorii logici
bool verifElLeg(char a) {
    char elLeg[] = {'~', 'n', 'v', '&', '-'};
    for (int i = 0; i < 5; i++) if (a == elLeg[i]) return 1;
    return 0;
}

// Verifica variabile
char verifVar(char a) {
    char var[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
    for (int i = 0; i < 10; i++) if (a == var[i]) return var[i];
    return '0';
}

// Prioritatea operatiilor
int prioritate(char op) {
    if (op == 'n') return 5; // Negatie
    if (op == '&') return 4; // Conjunctie
    if (op == 'v') return 3; // Disjunctie
    if (op == '-') return 2; // Implicatie
    if (op == '~') return 1; // Echivalenta
    return 0;
}

// Asociativitatea operatorilor logici (1 pentru asociativitate-stanga, -1 pentru asociativitate-dreapta)
int asociativitate(char op) {
    if (op == 'n') return -1; // asociativitate-dreapta
    return 1; // asociativitate-stanga pentru alti operatori logici
}

// Converteste forma infixata in forma postfixata (poloneza inversa) (Shunting Yard Algorithm)
std::string infixToPostfix(std::string infix) {
    std::stack<char> operatori;
    std::string output = "";
    for (char token : infix) {
        if (verifVar(token) != '0') {
            // Daca avem o variabila, o adaugam la stiva
            output += token;
        } else if (verifElLeg(token)) {
            // Daca e un operator
            while (!operatori.empty() && verifElLeg(operatori.top()) && //atata timp cat stiva de operatori nu e goala si operatorul logic din varf nu e paranteza
                   ((asociativitate(token) == 1 && prioritate(token) <= prioritate(operatori.top())) || //asociativ la stanga si operatorul logic actual nu este mai prioritar decat cel din varful stivei
                    (asociativitate(token) == -1 && prioritate(token) < prioritate(operatori.top())))) { //asociativ la dreapta si operatorul logic curent nu este mai prioritar strict decat cel din varful stivei
                output += operatori.top(); //ia operatorul logic care se afla in varful stivei de operatori si il adauga la output (forma postfixata)
                operatori.pop(); //sterge operatorul pe care tocmai l-am trimis la output din varful stivei de operatori
            }
            operatori.push(token); //adauga la stiva de operatori logici operatorul curent cu prioritate mai mica
        } else if (token == '(') { //daca el curent este paranteza deschisa
            operatori.push(token); //adauga la stiva de operatori logici '('
        } else if (token == ')') { //daca el curent e paranteza inchisa
            while (!operatori.empty() && operatori.top() != '(') { //atata timp cat stiva de operatori logici nu este goala si operatorul din varf e diferit de paranteza deschisa
                output += operatori.top(); //adauga la forma postfixata operatorul logic
                operatori.pop(); //sterge operatorul logic (care a fost adaugat la forma postfixata) din stiva
            } // daca se iese din while, ori stiva de operatori e goala, ori s-a gasit o paranteza deschisa
            operatori.pop(); // se sterge paranteza deschisa
        }
    }

    // introduce in forma postfixata operatorii logici ramasi in stiva
    while (!operatori.empty()) {
        output += operatori.top();
        operatori.pop();
    }

    return output;
}

// Evaluarea formei postfixate
int evaluarePostfix(const std::string &postfix, std::unordered_map<char, int> &values) {
    std::stack<int> evalStiva; // crearea stivei de evaluare a formulei

    for (char token : postfix) { // se iau elementele din forma postfixata de la stanga la dreapta
        if (verifVar(token) != '0') { // daca elementul urmator din forma postfixata este o variabila
            evalStiva.push(values[token]); // se introduce valoarea variabilei in stiva de evaluare, de la stanga la dreapta
        } else if (token == 'n') { // daca elementul urmator din forma postfixata este operatorul logic negatie
            int a = evalStiva.top(); // se initializeaza 'a' (variabila tip intreg) care va primi valoarea variabilei din varful stivei de evaluare
            evalStiva.pop(); // se elimina din stiva de evaluarea valoarea variabilei existente (pentru ca a fost deja salvata in 'a')
            evalStiva.push(neg(a)); // se neaga valoarea variabilei (cea care a fost in varful stivei si a fost salvata in a), iar rezultatul negatiei este introdus inapoi in stiva de evaluare
        } else { // daca elementul urmator din forma postfixata este oricare alt operator logic in afara de 'n' - negatie
            int b = evalStiva.top(); evalStiva.pop(); // se salveaza valoarea variabilei din varful stivei - 1st, in b (daca a+b, in forma post fixata vom avea ab+, dar in stiva de evaluare a formulei vom avea b, a)
            int a = evalStiva.top(); evalStiva.pop(); // se salveaza valoarea celei de a doua variabile din varful stivei - 2nd, in a
            if (token == '&') evalStiva.push(conj(a, b)); // daca operatorul din forma postfixata este &, se calculeaza conjunctia intre a si b, in ordinea asta
            else if (token == 'v') evalStiva.push(disj(a, b)); // daca operatorul din forma postfixata este v, se calculeaza disjunctia
            else if (token == '-') evalStiva.push(impl(a, b)); // daca operatorul din forma postfixata este -, se calculeaza implicatia
            else if (token == '~') evalStiva.push(echiv(a, b)); // daca operatorul din forma postfixata este ~, se calculeaza echivalenta
        }
    }

    return evalStiva.top(); // rezultatul final se afla in varful stivei
}

// se obtin informatii despre formula: care sunt variabilele distincte, numarul de aparitii al variabilelor, numarul de operatori, subformule, numarul de subformule
void getStats(const std::string &infix, int &subformule, int &numOperatori, std::map<char, int> &varCount) {
    std::set<char> distinctVars; // set - container (ca un vector) pentru elemente distincte (de tip char in cazul nostru) sortate, fara o limita a nrului de elemente retinute
    std::unordered_set<std::string> unicSubformule; // set pentru retinerea subformulelor. seturile, daca primesc duplicate, nu le iau in seama
    std::string postfixFormula = infixToPostfix(infix); // se converteste formula din forma infixata in forma postfixata

    std::stack<std::string> operandStiva; //stiva de operatii

    for (size_t i = 0; i < postfixFormula.length(); ++i) { // atata timp cat avem elemente in forma postfixata, se ia fiecare element, de la dreapta la stanga
        char token = postfixFormula[i]; // se salveaza elementul de pe pozitia i a stringului ce contine forma postfixata

        if (verifVar(token) != '0') { // daca elementul este o variabila
            // orice variabila unica este o subformula
            std::string var(1, token); // se creeaza o noua variabila de tip string, ce contine un singur caracter, in care se introduce variabila
            varCount[token]++; // se incrementeaza numarul de variabile curent
            distinctVars.insert(token); // se introduce in setul de variabile distincte variabila gasita (doar daca nu se regaseste deja printre elementele setului)

            operandStiva.push(var); // variabila este introdusa in stiva de operatii


            unicSubformule.insert(var); // se introduce variabila in setul de subformule (daca nu se regaseste deja)
        } else if (verifElLeg(token)) { // daca elementul este un operator logic
            if (token == '&' || token == 'v' || token == '-' || token == '~') { // daca operatorul logic este op de conjunctie, disjunctie, implicatie, echivalenta
                std::string dr = operandStiva.top(); operandStiva.pop(); // scoate variabilele a caror operatie este desemnata de operatorul token
                std::string stg = operandStiva.top(); operandStiva.pop();

               std::string subformula = stg + dr + token; // se formeaza subformula

                operandStiva.push(subformula); // se introduce subformula in stiva de subformule

                unicSubformule.insert(subformula); // se introduce subformula in setul de subformule unice
            } else if (token == 'n') { // daca operatorul logic este negatie
                std::string operand = operandStiva.top(); operandStiva.pop(); // se declara si initializeaza un string cu variabila din varful stivei de operatii, si se elimina acea variabila din stiva
                std::string subformula = operand + token; // se declara si initializeaza un string cu variabila si operatorul logic negatie

                operandStiva.push(subformula); // se introduce duo-ul variabila/negatie in stiva de operatii
                unicSubformule.insert(subformula); // si in setul de subformule unice
            }
            numOperatori++; // se incrementeaza numarul de operatori logici
        }
    }

    subformule = unicSubformule.size(); // Numarul de subformule este reprezentat de dimensiunea setului de subformule unice
}

// Functia principala ce rezolva ecuatia
int FF(std::string formula, std::unordered_map<char, int> valori) {

    std::string postfix = infixToPostfix(formula); // Converteste formula din forma sa infixata in forma sa postfixata
    //std::cout << "Postfix notation: " << postfix << std::endl;


    int result = evaluarePostfix(postfix, valori); // se evalueaza forma postfixata pentru un anumit set de valori ale variabilelor
    return result;
}

// Functie pentru extragerea variabilelor distincte din formula
std::set<char> getDistinctVariables(const std::string& formula) {
    std::set<char> distinctVars; // declarare set-container pentru retinerea variabilelor distincte
    for (char token : formula) { // se parcurge formula, de la stanga la dreapta, element cu element
        if (verifVar(token) != '0') { // daca elementul curent este variabila
            distinctVars.insert(token); // se introduce acest element in setul de variabile distincte
        }
    }
    return distinctVars;
}

// Afiseaza tabelul de adevar pentru formula
void afisareTabelAdevar(const std::string& formula) {
    std::set<char> distinctVars = getDistinctVariables(formula); // declarare si initializare set cu variabilele distincte din formula
    int numVars = distinctVars.size(); // numarul de variabile distincte reprezinta dimensiunea setului ce tine variabilele distincte
    std::vector<char> varList(distinctVars.begin(), distinctVars.end()); // declara si initializeaza un vector de tip char, copiind toate elementele setului de variabile distincte distinctVars

    // nr total de randuri pentru tabelul de adevar este 2^n (n - nr de variabile)
    int nrRanduri = 1;
    for (int i = 0; i < numVars; ++i) {
    nrRanduri *= 2; // 2^n numar de randuri sau 2^numVars
    }
    // se poate inlocui forul cu totalRows = pow(2, numVars);  -   #include <cmath>
    // se afiseaza capul tabelului
    for (char var : varList) { // se citesc variabilele din vectorul de variabile atata timp cat avem variabile de citit
        std::cout << var << " "; // se afiseaza variabilele pentru capul de tabel
    }
    std::cout << " FF" << std::endl; // dupa ce se termina variabilele, afisam si capul coloanei pentru rezultatul functiei
    std::cout <<"----------" << std::endl;
    for (int i = 0; i < nrRanduri; i++) { // se trece prin toate randurile de tabel

        std::unordered_map<char, int> valVar; // se declara o mapa neordonata care asociaza fiecarei variabile distincte char (A,B,..) o variabila int pentru randul curent din tabel
        for (int j = 0; j < numVars; j++) {
            int divizor = 1; // variabila cea mai din stanga isi schimba valoarea la fiecare 2^(n-1) (A la fiecare 8 randuri) randuri, urmatoarea la 2^(n-2) (B la fiecare 4 randuri) ,...
            for (int k = 0; k < (numVars - j - 1); k++) { // k - contor pentru multiplicarea repetata a divizorului cu 2
                divizor *= 2;
        } // == divizor=pow(2,numVars-j-1)
        int valBit = (i / divizor) % 2; // se atribuie o valoare de adevar (0 sau 1) pentru variabila 'j', de pe randul curent 'i'

        /*
        pentru randul 4, i=3
        A: divizor = 8
            valoareBit=(3/8)%2=0%2=0 -> A=0
        B: divizor = 4
            valoareBit=(3/4)%2=0%2=0 -> B=0
        C: divizor = 2
            valoareBit=(3/2)%2=1%2=0 -> C=1
        D: divizor = 1
            valoareBit=(3/1)%2=3%2=0 -> D=1
        pentru randul 3, vom avea:
        A B C D
        0 0 1 1
        */
        valVar[varList[j]] = valBit; // se salveaza valoarea variabilei j pentru randul i in mapa
    }

        // se calculeaza ecuatia pentru valorile de pe randul i
        int result = FF(formula, valVar);

        // se afiseaza valorile variabilelor pentru randul i
        for (char var : varList) {
            std::cout << valVar[var] << " ";
        }

        std::cout <<" "<< result << std::endl; // se afiseaza rezultatul ecuatiei pentru valorile de pe randul i
    }
}

int main() {
    std::string formula = "(nCv(((nAv(nD~A))v(D-(C-(B&A))))~(nD-nB)))";

    int subformule = 0, numOperatori = 0;

    std::map<char, int> varCount;

    getStats(formula, subformule, numOperatori, varCount);

    std::cout << "Problema 47: " << std::endl;
    std::cout << "INPUT: " << std::endl <<"----------"<< std::endl;
    std::cout << "Formula: " << formula << std::endl;
    std::cout << "\tunde A, B, C, D sunt variabile," << std::endl;
    std::cout << "\t\t- = implication " << std::endl;
    std::cout << "\t\t~ = equivalence " << std::endl;
    std::cout << "\t\t& = conjunction " << std::endl;
    std::cout << "\t\tv = disjunction " << std::endl;
    std::cout << "\t\tn = negation " << std::endl << std::endl;

    std::cout << "OUTPUT: " << std::endl <<"----------"<< std::endl;
    std::cout << "a. Numar de subformule (unice, unde formula este o subformula a sa): " << subformule << std::endl;
    std::cout << "b. Numar de variabile distincte: " << varCount.size() << std::endl;
    std::cout << "c. Numar de operatori logici: " << numOperatori << std::endl;

    std::cout << "d. Numar de aparitii al fiecarei variabile:" << std::endl;
    for (const auto &var : varCount) {
        std::cout << var.first << ": " << var.second << "\t";
    }
    std::cout << std::endl << std::endl;


    std::cout << "e. Tabel de adevar:\n\n";
    afisareTabelAdevar(formula);
    std::cout <<"----------" << std::endl;

    return 0;
}
