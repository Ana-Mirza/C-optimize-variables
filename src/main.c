/* 
The MIT License (MIT)

Copyright (c) 2020 Anna Brondin and Marcus Nordström

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitatioceiln the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#define DEBUG
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "StepCountingAlgo.h"
#include "scoringStage.h"
#include "detectionStage.h"
#include "postProcessingStage.h"
#include "motionDetectStage.h"

#define nitems(_a)    (sizeof(_a) / sizeof(0[(_a)]))

struct datapoint
{
	char* path;    /* the data of register */
	int steps;      /* the unit is us */
};

static const struct datapoint dataset[] = {
    {"../Dataset/optimize/csv_data/leganat_normal.csv", 150},
    {"../Dataset/optimize/csv_data/leganat_incet.csv", 150},
    {"../Dataset/optimize/csv_data/leganat_rapid.csv", 150},
    {"../Dataset/optimize/csv_data/leganat_alergat.csv", 150},
    {"../Dataset/optimize/csv_data/buzunar.csv", 170},
    {"../Dataset/optimize/csv_data/stationar.csv", 150},
    {"../Dataset/optimize/csv_data/scari.csv", 150},
};

static void runAlgo(char* path)
{
    char *line = NULL;
    size_t len = 0;
    size_t read;
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1)
    {
        long time = strtol(strtok(line, ","), NULL, 10);
        long x = (strtol(strtok(NULL, ","), NULL, 10));
        long y = (strtol(strtok(NULL, ","), NULL, 10));
        long z = (strtol(strtok(NULL, ","), NULL, 10));
        processSample(time, x, y, z);
    }
    fclose(fp);
    if (line)
        free(line);
}

int main(int argc, char *argv[])
{
    int global_error = 10000;

    initAlgo();

    for (int16_t motionThreshold = 100; motionThreshold <= 300; motionThreshold += 25)
    {    
        for (size_t scoringWindowSize = 20; scoringWindowSize <= 30; scoringWindowSize++)
        {
            for (int detectionThreshold_whole = 1; detectionThreshold_whole <= 5; detectionThreshold_whole++)
            {
                for (int detectionThreshold_frac = 1; detectionThreshold_frac <= 9; detectionThreshold_frac++)
                {
                    for (int postTimeThreshold = 100; postTimeThreshold <= 500; postTimeThreshold += 5)
                    {
                        int error = 0;
                        for (int idx = 0; idx < nitems(dataset); idx++)
                        {
                            int correct_answer = dataset[idx].steps;

                            changeWindowSize(scoringWindowSize);
                            changeDetectionThreshold(detectionThreshold_whole, detectionThreshold_frac);
                            changeTimeThreshold(postTimeThreshold);
                            changeMotionThreshold(motionThreshold);
                            runAlgo(dataset[idx].path);

                            /* Add datapoint error to global error */
                            error += abs(getSteps() - correct_answer);

                            resetSteps();
                            resetAlgo();
                        }

                        /* Find best parameter combination across datapoints */
                        if (error <= global_error) {
                            global_error = error;
                            printf("Error=%d windowSize=%zu threshold=%d+1/%d timethreshold=%d motionthreshold=%d\n", 
                                global_error, 
                                scoringWindowSize, 
                                detectionThreshold_whole, 
                                detectionThreshold_frac, 
                                postTimeThreshold,
                                motionThreshold);
                            fflush(stdout);
                        }
                    }
                }
            }
        }
    }
}