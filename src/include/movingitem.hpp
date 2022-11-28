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
            nextPosition.x *= percentageOfFinalTime;
            nextPosition.y *= percentageOfFinalTime;
            item.moveItem(nextPosition + startingPosition);
        }
        return true;
    }

    Item& getItem(){
        return item;
    }
};