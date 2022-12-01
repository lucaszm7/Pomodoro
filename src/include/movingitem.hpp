#pragma once
#include "olcPixelGameEngine.h"
#include "item.hpp"

class MovingItem {
private:
    olc::vi2d startingPosition;
    olc::vi2d finalPosition;
    uint32_t timeToReachTargetPosition = 500;
    Item item;
    float fElapsedTime = 0;

public:
    MovingItem (Item item_to_animate, olc::vi2d finalPosition, uint32_t milisecconds) 
    : finalPosition(finalPosition), item(item_to_animate), timeToReachTargetPosition(milisecconds), startingPosition(item_to_animate.getLeftUpperCorner())
    {
    }

    MovingItem (Item item_to_animate, olc::vi2d finalPosition) 
    : finalPosition(finalPosition), item(item_to_animate), startingPosition(item_to_animate.getLeftUpperCorner())
    {
    }

    bool isInFinalPosition() const {
        return finalPosition == item.getLeftUpperCorner();
    }

    bool move(float timeFromLastUpdate){
        fElapsedTime += timeFromLastUpdate;
        if (fElapsedTime >= timeToReachTargetPosition){
            item.moveItem(finalPosition);
        }
        else {
            double percentageOfFinalTime = (double) (fElapsedTime / timeToReachTargetPosition);
            olc::vi2d nextPosition = (finalPosition - startingPosition);
            // nextPosition.x *= sin(percentageOfFinalTime*3.14 / 2.0) * cos(percentageOfFinalTime*3.14 - 3.14);
            // nextPosition.y *= sin(percentageOfFinalTime*3.14 / 2.0) * cos(percentageOfFinalTime*3.14 - 3.14);
            // nextPosition.x *= cos(percentageOfFinalTime*3.14 * 2.0 ) / 2.0 + 0.5;
            // nextPosition.y *= sin(percentageOfFinalTime*3.14 / 2.0);
            // nextPosition.x *= -1 * ((percentageOfFinalTime - 1) * (percentageOfFinalTime - 1) * (percentageOfFinalTime - 1) * (percentageOfFinalTime - 1)) + 1;
            
            // x moves by (x - 1) ^ 3 + 1
            // y moves by (y) ^ 3
            // this mens that the x value moves faster at the begining and slowly ant the end
            // while the y moves slow at the begining of the animation and moves faster at the end
            nextPosition.x *= ((percentageOfFinalTime - 1) * (percentageOfFinalTime - 1) * (percentageOfFinalTime - 1) + 1);
            nextPosition.y *= percentageOfFinalTime*percentageOfFinalTime*percentageOfFinalTime;
            item.moveItem(nextPosition + startingPosition);
        }
        return true;
    }

    Item& getItem(){
        return item;
    }
};