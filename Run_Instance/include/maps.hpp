#pragma once

struct Map {
    int PLACEHOLDERVALUE_CHANGE;
};

// Function to generate map
Map makeMap(int seed);

// Function to alter a passed map
Map alterMap(Map oldMap);
