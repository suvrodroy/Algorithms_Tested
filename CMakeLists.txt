cmake_minimum_required(VERSION 3.25)
project(Algorithms_Tested)

set(CMAKE_CXX_STANDARD 17)

add_executable(Algorithms_Tested
#        "Finding Bridges in Graphs.cpp"
#        "Find Cycle.cpp"
#        "Lowest Common Ancestor.cpp"
#        main.cpp
#        "Monotone Chain Convex Hull.cpp"
        AhoCorassic.cpp
        GeoNecessities.cpp
        DynamicConvexHull.cpp
        suffixArray.cpp
        Temp.cpp
        BitTrie.cpp
        SuffixAutomaton.cpp
        Geo.cpp
)