#ifndef INSTRUCTIONCACHE_H
#define INSTRUCTIONCACHE_H

#include "cache.h"

class Processor;

class InstructionCache : public Cache
{
private:
    /// Matriz con los bloques de la caché de instrucciones.
    /// Cada instrucción está compuesta de 4 enteros, para efectos de la simulación, para no tener que hacer operaciones de bits.
    int cacheMem[4][4][4] = {{{0}}};
public:
    InstructionCache();

    /**
     * @brief isInstructionInCache verifica si el bloque al que pertenece una instrucción se encuentra en la caché.
     * @param blockInMemory número de bloque correspondiente en memoria.
     * @param blockInCache posición en la que se encontraría el bloque que se está verificando.
     * @return true si el bloque en caché corresponde al bloque en memoria. False si ocurre lo contrario.
     */
    inline bool isInstructionInCache(const int &blockInMemory, const int &blockInCache)
    {
        return blockID[blockInCache] == blockInMemory;
    }

    /**
     * @brief fetch copia en instruction la instrucción apuntada por el pc de processor. Resuelve el fallo de caché si una instrucción no se encuentra en caché.
     * @param processor puntero al procesador que es dueño de esta caché. Se utiliza para obtener el valor del pc y para pasarle el puntero a solveFail.
     * @param instruction arreglo donde se copian los cuatro enteros que forman una instrucción.
     */
    void fetch(Processor* processor, int instruction[4]);

    /**
     * @brief solveFail resuelve fallos de caché de instrucciones. Copia en la caché y actualiza el blockID del siguiente bloque que se quiere subir.
     * @param processor puntero al procesador que es dueño de esta caché. Se utiliza para acceder a la memoria de instrucciones.
     * @param blockInMemory número de bloque en memoria de instrucciones.
     * @param blockInCache posición del bloque en caché.
     */
    void solveFail(Processor* processor, const int &blockInMemory, const int &blockInCache);
};

#endif // INSTRUCTIONCACHE_H
