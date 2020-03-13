
#ifndef CASSETE_H
#define CASSETE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ConstructionContainer.h"
#include "gui.h"

class CasseteCell{
public:
    CasseteCell(ConstructionContainer * container);
    void attachBond(struct BondLocation bond);
    bool getBond(const glm::mat4 * matr, bool pos_test = true);
    ConstructionContainer * container;
    glm::mat4 matr;
    bool noempty;
};

class Cassete{
public:
    CasseteCell * places[8]; 

    Cassete(ConstructionContainer * container);
    ~Cassete();

    void fillUp();
    int getBond(const glm::mat4 * matr);///<этот метод будет вызываться с матрицей заклепочника манипулятора
    void updateMatrices(const glm::mat4 * parent);
    protected:
    bool pos_test_var;

};

class GUICassete: public Cassete, public GUIWindow{
public:
    GUICassete(nanogui::Screen * screen, ConstructionContainer * container);
    ~GUICassete();
    void doStep();
    int getBond(const glm::mat4 * matr, bool);
    private:
    nanogui::CheckBox * pos_test;
};

#endif

