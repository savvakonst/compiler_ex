
#include "table.h"
#include "variable.h"

Block::Block (Variable* var) {
    level=var->getLevel();
    setUint(var);
}
void    Block::setUint(Variable * var)
{
    unitList.push(var);

}

string  Block::print() {
    string out="\tlevel: " + std::to_string(level) + "\n";
    for (auto i : unitList) {
        out+="\t\t" + i->printUint() + ";" + (i->isBuffered()?" store\n" :"\n");
    }
    return out;
}



//TableColumn::TableColumn section 
//
//
TableColumn::TableColumn (Variable* var) {
    length =var->getLength();
    setUint(var);
}
void    TableColumn::setUint(Variable * var){
    auto  varlevel= var->getLevel();
    for (auto i : blockList) 
        if (i->getLevel() == varlevel) {
            i->setUint(var);
            return;
        }
    blockList.push(new Block(var));
}

string  TableColumn::print(){
    string out="length: " + std::to_string(length) + "\n";
    for (auto i : blockList) {
        out+= i->print();
    }
    return out;
}




//Table::Table section 
//
//
void    Table::setUint(Variable * var)
{
    auto  varLength= var->getLength();
    if (isConst(var)) {
        constList.push(var);
        return;
    }
    if (isSmallArr(var)) {
        smallArrayList.push(var);
        return;
    }
    for (auto i : columnList)
        if (i->getLength() == varLength) {
            i->setUint(var);
            return;
        }
    columnList.push(new TableColumn(var));
}

string  Table::print() {
    string out="";

    out+="constant defenition:\n";
    for (auto i : constList) {
        out+="\t" + i->printUint() + ";\n";
    }
    out+="small array defenition:\n";
    for (auto i : smallArrayList) {
        out+="\t" + i->printUint() + ";\n";
    }
    out+="table:\n";
    for (auto i : columnList) {
        out+= i->print();
    }

    std::cout << out;
    return out;
}