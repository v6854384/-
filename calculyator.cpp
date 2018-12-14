#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

using namespace std;

class Stack
{
    public:
    vector<char> Operators;
    vector<int> Priorities;
    vector<double> Operands;
    map<string, double> VarTable; // Таблица переменных
    vector<string> ProgBuf; // Буфер строк с вложенной программой
    string FinVal; // Имя переменной, куда записывается результат
    bool coutF=true; // Флаг разрешения вывода финального значения
    int ProgBracked=0; // счетчик программных скобок во время буферизации
    int Priority=0; // Приоритет операции
    int StackCalc(double Var, char Op, char Op2);
    bool Bufering(string str); // Буферизация программы
    bool ProgExecute(vector<string> Buf);
    double Expr(string expr);
    int S;
    bool Error=false;
};

bool down(vector<string> Buf, int &i) // Спуск по списку команд до end или else
{
    int prior=0;
    for(; i<Buf.size() && prior>=0;i++)
        if(Buf[i].find("while")==0 || Buf[i].find("for")==0 || Buf[i].find("if")==0)
            prior++;
        else
            if(Buf[i].find("else")==0 && prior==0)
                return true;
            else
                if(Buf[i].find("end")==0)
                    if(prior==0)
                        return true;
                    else
                    {
                        prior--;
                        if(prior<0) return false;
                    }
    return false;
}

bool Stack::ProgExecute(vector<string> Buf) // выполнение программы из буфера
{
    int i=0;

    for(int i=0;i<Buf.size();i++) // Преобразование for в while
        if(Buf[i].find("for")==0)
            {

            }

    vector<int> LineBack;// Стек номеров строк для возвращения по завершению программнго блока
    for(i=0;i<Buf.size();)
        if(Buf[i].find("while")==0)
        {
            {
            if(Buf[i].length()==5) {Error=true; return false;}
            coutF=false;
            if(Expr(Buf[i].substr(5,1000)))
                {LineBack.push_back(i++);/*cout<<"iteratin "<<endl*/}
            else
                {
               // cout<<"beg of down\n";
                if(!down(Buf,++i)) {Error=true; return false;}
                else i++;
               // cout<<"while down is created!\n";
                }
            coutF=true;
            }
        }
        else if(Buf[i].find("if")==0)
        {   coutF=false;
            if(Expr(Buf[i].substr(2,1000)))
                {LineBack.push_back(-1); i++;} // флаг завершения блока до else
            else
                {
                i++;
                if(!down(Buf,i)) return false;
                if(Buf[i].find("else")==0)
                    {
                        LineBack.push_back(-1); // флаг завершения блока после else
                        i++;
                    }
                }
            LineBack.push_back(-1);
            coutF=true;
            // Пропустить до end или до else если ложь
        }
        else if(Buf[i].find("end")==0)
        {
            if(LineBack.size()==0) {Error=true; cout<<"End without if, while, for"; return false;}
            if(LineBack.back()>=0)
                {i=LineBack.back(); LineBack.pop_back(); /*cout<<"back to "<<i<<endl;*/}
            else
                if(LineBack.back()<0){LineBack.pop_back();i++;}
        }
        else if(Buf[i].find("else")==0)
        {
            if(LineBack.back()<0)
                {
                    if(!down(Buf,i) || Buf[i].find("end")!=0) return false;
                    i++;
                }
        }
        else {Expr(Buf[i]);i++;}
}

bool Stack::Bufering(string str) // Буферизация программы (формирование списка команд)
{
    int i=0;
    while(str[i]==' ')i++;
    str=str.substr(i,10000); // Удаляем пробелы в начале строки
    if(str.find("for")==0 || str.find("while")==0 || str.find("if")==0)
        {
            ProgBuf.push_back(str);
            ProgBracked++;
        }
    else if(str.find("end")==0)
    {
        if(ProgBracked==0) return false;
        ProgBuf.push_back(str);
        ProgBracked--;
        if(ProgBracked==0) // Вывполнение программного блока
        {
           // for(i: ProgBuf)
           //     cout<<i<<endl;
            ProgExecute(ProgBuf); // Выполенние программы из списка
            ProgBuf.clear();
            return true;
        }
    }
        else if(ProgBracked==0)
                Expr(str);
            else
                ProgBuf.push_back(str);
    return true;
}

int Stack::StackCalc(double Var, char Op, char Op2) // Добавление операнда и оператора в стек
// Возвращается коррекция номера позиции (при операции из двух символов коррекция +1
{
    Operands.push_back(Var);
    int correct=0;
    switch(Op)
    {
        case '>': if(Op2=='=') Op='^'; break; // Операция >=
        case '<': if(Op2=='=') Op='v'; break; // Операция <=
        case '!': if(Op2!='=') {Error=true; return 0;} break; // Операция !=
        case '=': if(Op2!='=') {Error=true; return 0;} break; // Операция !=
    }
    int t=Priority;
    if(Op!=0) // Пустая операции (просто добавление операнда в стек операндов
        {
        switch(Op) // Опред
            {
            case   0: t=0; break;
            case '+':
            case '-': t+=1; break;
            case '*':
            case '/': t+=2; break;
            case '>':
            case '<':
            case '=':
            case '!':
            case '^':
            case 'v': t+=3; break;

            default: cout<<"Syntax error"; Error=true;
            }
        }
    while(t<=Priorities.back())
    {
        double var1=Operands.back(); Operands.pop_back();
        double var2=Operands.back(); Operands.pop_back();
        switch(Operators.back()) // Выполнение операции
        {
            case '+': Operands.push_back(var1+var2); break;
            case '-': Operands.push_back(var2-var1); break;
            case '*': Operands.push_back(var1*var2); break;
            case '/': Operands.push_back(var2/var1); break;
            case '>': Operands.push_back(var2>var1); break;
            case '^': Operands.push_back(var2>=var1); break;
            case '<': Operands.push_back(var2<var1);  break;
            case 'v': Operands.push_back(var2<=var1); break;
            case '=': Operands.push_back(var2==var1); break;
            case '!': Operands.push_back(var2!=var1); break;
        }
        Priorities.pop_back();
        Operators.pop_back();
    }
    if(Op!=0)
        {Operators.push_back(Op); Priorities.push_back(t);}
    switch(Op)
    {
        case '^':
        case 'v':
        case '!':
        case '=': return 1;
    }
    return 0;
}

double Stack::Expr(string expr) // Возвращается вычисленный реузльтат
{
    Error=false;
    string VarBuf, FigureBuf;
    Priorities.clear();
    Priorities.push_back(-1); // Символ конца стека
    //Operators.push_back(0);
    S=0;
    Priority=0;
    VarBuf="";
    FinVal=""; // переменная для результата

    for(int i=0;i<=expr.length() && S>=0 && !Error;i++)
        {
            while(expr[i]==' ')i++;
            switch(S)
        {
            case 0:
//                cout<<"Stage 0\n";
                if(expr[i]>='0' && expr[i]<='9' || expr[i]=='-')
                {S=4;  FigureBuf=expr[i];}
                else if(expr[i]>='a' && expr[i]<='z' || expr[i]>='A' && expr[i]<='Z')
                {S=1; VarBuf=expr[i];}
                else if(expr[i]=='(')
                    {S=2; Priority+=10;}
                else
                    {cout<<"Expression error\n"; return false;}
                break;
            case 1: // Распознание первого идентификатора (переменной, которой надо присваивать значение)
//                cout<<"Stage 1\n";
                if(expr[i]>='a' && expr[i]<='z' || expr[i]>='A' && expr[i]<='Z')
                    VarBuf+=expr[i];
                else
                    if(expr[i]==0)
                    {
                    // Вывод переменной на экран
                    if(VarTable.count(VarBuf)==0)
                        {cout<<"Unnoun variable\n"; return false;}
                    else
                        {cout<<VarTable[VarBuf]<<endl; return VarTable[VarBuf];}
                    }
                else
                    if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/' || expr[i]=='<' || expr[i]=='>' || expr[i]=='!' || expr[i]=='=')
                    {

                    if(expr[i]=='=' && expr[i+1]!='=')
                        {FinVal=VarBuf; if(!VarTable.count(FinVal)) VarTable[FinVal]=0;}
                    else
                        if(VarTable.count(VarBuf)==0)
                            {cout<<"Unnoun variable\n"; return false;}
                        else
                            i+=StackCalc(VarTable[VarBuf],expr[i], expr[i+1]);

                    VarBuf="";
                    S=2;
                    }
                break;
            case 2:
//                cout<<"Stage 2\n"<<expr[i]<<endl;
                if(expr[i]>='a' && expr[i]<='z' || expr[i]>='A' && expr[i]<='Z')
                    {S=3; VarBuf=expr[i];}
                else if(expr[i]>='0' && expr[i]<='9' || expr[i]=='-')
                    {S=4; FigureBuf=expr[i];}
                else if(expr[i]=='(')
                    Priority+=10;
                else
                    {cout<<"Expression error 2\n"; return false;}
                //S=-1;
                break;
            case 3: // Распознание идентификатора
//                cout<<"Stage 3\n";
                if(expr[i]>='A' && expr[i]<='Z' || expr[i]>='a' && expr[i]<='z' || expr[i]>='0' && expr[i]<='9')
                    VarBuf+=expr[i];
                else
                   if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/' || expr[i]=='<' || expr[i]=='>' || expr[i]=='!' || expr[i]=='=' || expr[i]==0)
                        {
                        if(VarTable.count(VarBuf)==0) {cout<<"Unnoun variable\n"; return false;}
                        i+=StackCalc(VarTable[VarBuf],expr[i], expr[i+1]);
                        VarBuf="";
                        S=2;
                        }
                   else
                        if(expr[i]==')')
                            {if(VarTable.count(VarBuf)==0) {cout<<"Unnoun variable\n"; return false;}
                            S=7; FigureBuf=to_string(VarTable[VarBuf]); Priority-=10; VarBuf="";}
                        else
                            {cout<<"Expression error 3\n"; return false;}
                break;
            case 4: // распознание константы
//                cout<<"Stage 4\n";
                if(expr[i]>='0' && expr[i]<='9')
                    FigureBuf+=expr[i];
                else
                    if(expr[i]=='.')
                        {FigureBuf+=expr[i]; S=5;}
                    else
                        if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/' || expr[i]=='<' || expr[i]=='>' || expr[i]=='!' || expr[i]=='=' ||  expr[i]==0)
                        {
                            if(expr[i]==0 && Priority!=0)
                            {
                                cout<<"Bracket error\n";
                                return false;
                            }
                            i+=StackCalc(atof(FigureBuf.c_str()),expr[i], expr[i+1]);
                            S=2;
                        }
                    else if(expr[i]==')')
                            {S=7;  Priority-=10;}
                        else
                            S=-1;
                break;
            case 5:
                if(expr[i]>='0' && expr[i]<='9')
                    {FigureBuf+=expr[i]; S=6;}
                else
                    S=-1;
                break;
            case 6: // Распознения числа после десятичной точки
                if(expr[i]>='0' && expr[i]<='9')
                    FigureBuf+=expr[i];
                else
                    if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/' || expr[i]=='<' || expr[i]=='>' || expr[i]=='!' || expr[i]=='=' ||  expr[i]==0)
                    {
                        i+=StackCalc(atof(FigureBuf.c_str()),expr[i], expr[i+1]);
                        S=2;
                    }
                else
                    S=-1;
                break;
            case 7: // Распознание закрывающихся скобок
//                cout<<"Stage 7\n";
                if(Priority<0)
                      {cout<<"Bracket error\n"; return false;}
                if(expr[i]==')')
                {
                    Priority-=10;
                    if(Priority<0)
                      {
                          cout<<"Bracket error\n";
                          return false;
                      }
                }
                else if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/' || expr[i]=='<' || expr[i]=='>' || expr[i]=='!' || expr[i]=='=' || expr[i]==0)
                    {i+=StackCalc(atof(FigureBuf.c_str()),expr[i], expr[i+1]);S=2;}
                else
                    {
                    cout<<"Expression error\n";
                    return false;
                    }
                break;
        }
        }
    if(S==-1)
        {cout<<"Expression error!!!\n"; return false;}
    if(Priority!=0)
        {cout<<"Bracket error\n"; return false;}
    else if(FinVal=="")
        {if(Operands.size()!=0 && coutF) cout<<Operands.back()<<endl;}
    else
      {VarTable[FinVal]=Operands.back();FinVal="";}
    return Operands.back();
    while(Operands.size()!=0) Operands.pop_back();
    return true;
}

int main()
{
    Stack St;
    string Line;
    while(getline(cin,Line), Line!="")
    {
        St.Bufering(Line);
    }
//    cout<<St.S;
    return 0;
}
