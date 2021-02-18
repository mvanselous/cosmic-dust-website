/*
Author: Kidus Adugna
Supervisor: Dr. Ludmilla Kolokolova
Date: 2/08/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <dirent.h>
#include <math.h>
#define SIZES_LEN 15
#define MATERIALS_LEN 14
#define MAX_LEN 300
#define ANGLES_LEN 180
#define MAX_SIZE 3000

/*Prototypes*/
int find_polarization(FILE * fp, float* polarization, float* brightness, const char* material_1, const char* color_1,  float size_1, int angle, char* original_path);
int find_size_r_two(FILE * fp, char* material_1, char* material_2, char* original_path, FILE ** current_file_1_blue, FILE ** current_file_1_red, FILE ** current_file_2_blue, FILE ** current_file_2_red);
int find_size_r_two_helper(FILE * fp, char* material_1, char* material_2, int angle, FILE *current_file_1_blue, FILE *current_file_1_red, FILE *current_file_2_blue, FILE *current_file_2_red, float** array_file_1_red, float** array_file_1_blue, float** array_file_2_red, float** array_file_2_blue);
int find_size_r_two_helper_2(FILE * fp, char* material_1, char* material_2, char* original_path, float color, float color_range, float polarization, float pola_range, int angle, float precision, float** array_file_1_red, float** array_file_1_blue, float** array_file_2_red, float** array_file_2_blue);

int main(int argc, char* argv[])
{
    int i, material_check = 0, material_check_2 = 0;
    char command[MAX_LEN], color_1[MAX_LEN], color_in_full[MAX_LEN], color_2[MAX_LEN], material_1[MAX_LEN], material_2[MAX_LEN];
    char *original_path = "./DATA 2D";
    int angle = 0, result_1 = 0, result_2 = 0, result_3 = 0, result_4 = 0;
    float j, size_1 = 0, size_2 = 0, step = 0, total_polarization = 0, s11 = 0;
    float polarization_1 = 0, polarization_2 = 0, brightness_2 = 0, color = 0, pola_range = 0, color_range = 0;
    float precision = 0, brightness_1 = 0;
    char materials[MAX_LEN][MAX_LEN] = {"Halley_non_pronus", "Halley_porous", "Halley+ice_0.70_volume", "Halley+ice_0.80_volume", "Halley+ice_0.90_volume", "Ice", "Ice_tholin", "ice+void_porosity_0.40", "ice+void_porosity_0.50", "ice+void_porosity_0.55", "Organic", "Silicate", "Titan_tholin", "Fayalite"};
    FILE *fp = NULL, *current_file_1_blue = NULL, *current_file_1_red = NULL, *current_file_2_blue = NULL, *current_file_2_red = NULL;//"ice+void_porosity_0.90",
    float** array_file_1_blue, **array_file_1_red, **array_file_2_blue, **array_file_2_red;

    fp = fopen("./outfile.csv", "w");
    if (argc == 5)
    {
        //Read input
        sscanf(argv[1],"%s", command);
        if (!strcmp(command, "find_color"))
        {
            sscanf(argv[2]," %s", material_1);
            size_1 = atof(argv[3]);
            angle = atoi(argv[4]);

            for (i = 0; i < MATERIALS_LEN; i++)
            {
                if (!strcmp(materials[i], material_1))
                {
                    material_check = 1;
                }
            }
            if (!material_check)
            {
                fprintf(fp, "%s is not a valid material. Please enter a valid material.\n", material_1);
                fclose(fp);
                exit(1);
            }

            if(size_1 <= 0 || size_1 >= 6)
            {
                fprintf(fp, "Invalid size. Please enter a size strictly between 0 and 6.\n");
                fclose(fp);
                exit(1);
            }
            if (angle < 0 || angle > 180)
            {
                fprintf(fp, "Please enter an angle between 0 and 180.\n");
                fclose(fp);
                exit(1);
            }
            //find polarization assuming we are using the blue filter.
            strcpy(color_1, "B");
            result_1 = find_polarization(fp, &polarization_1, &brightness_1, material_1, color_1, size_1, angle, original_path);
            //find polarization assuming we are using the red filter.
            strcpy(color_2, "R");
            result_1 = find_polarization(fp, &polarization_1, &brightness_2, material_1, color_2, size_1, angle, original_path);
            //Find color using the formula for color.
            color = -2.5 * log10((brightness_2 / brightness_1));
            fprintf(fp, "Material, Size, Angle, Color\n");
            fprintf(fp, "%s, %f, %d, %f\n", material_1, size_1, angle, color);
        }

        else
        {
            fprintf(fp, "Wrong command, Please try again.\n");
            fclose(fp);
            exit(1);
        }
    }

    else if (argc == 6)
    {
        sscanf(argv[1]," %s", command);
        if ((!strcmp(command, "find_pola")) || !strcmp(command, "find_bright"))
        {
            sscanf(argv[2]," %s", material_1);
            size_1 = atof(argv[3]);
            angle = atoi(argv[4]);
            sscanf(argv[5]," %s", color_1);

            if (!strcmp(color_1, "B"))
            {
                strcpy(color_in_full, "Blue");
            }
            else
            {
                strcpy(color_in_full, "Red");
            }
            fprintf(fp, "Material, Color, Angle, Polarization, Brightness\n %s, %s, %d", material_1, color_in_full, angle);

            for (i = 0; i < MATERIALS_LEN; i++)
            {
                if (!strcmp(materials[i], material_1))
                {
                    material_check = 1;
                }
            }
            if (!material_check)
            {
                fprintf(fp, "\n\n %s is not an invalid material. Please enter a valid material.\n", material_1);
                fclose(fp);
                exit(1);
            }

            if(size_1 <= 0)
            {
                fprintf(fp, "\n\n Invalid size. Please enter a positive size.\n");
                fclose(fp);
                exit(1);
            }
            if (angle < 0 || angle > 180)
            {
                fprintf(fp, "\n\n Please enter an angle between 0 and 180.\n");
                fclose(fp);
                exit(1);
            }
            if (strcmp("B", color_1) && strcmp("R", color_1))
            {
                fprintf(fp, "\n\n Invalid filter. Please enter 'B' for blue and 'R' for red.\n");
                fclose(fp);
                exit(1);
            }
            //find polarization and brightness of a single material
            result_1 = find_polarization(fp, &polarization_1, &brightness_1, material_1, color_1, size_1, angle, original_path);
            fprintf(fp, ",%f, %f", polarization_1, brightness_1);
            if (result_1)
            {
                fprintf(fp, ",%f, %f", polarization_1, brightness_1);
                fclose(fp);
                exit(1);
            }
        }
        else
        {
            fprintf(fp, "Wrong command, Please try again.\n");
            fclose(fp);
            exit(1);
        }
    }

    else if (argc == 8)
    {
        sscanf(argv[1]," %s", command);
        if (!strcmp(command, "find_size_r_two"))
        {
            polarization_1 = atof(argv[2]);
            pola_range = atof(argv[3]);
            color = atof(argv[4]);
            color_range = atof(argv[5]);
            angle = atoi(argv[6]);
            precision = atof(argv[7]);

            if (fabs(polarization_1) > 1)
            {
                fprintf(fp, "\n Invalid polarization. Please try again.\n");
                fclose(fp);
                exit(1);
            }
            if (pola_range <= 0 || pola_range >= 1)
            {
                fprintf(fp, "\n Invalid Pola range. Please provide a range between 0 and 1.\n");
                fclose(fp);
                exit(1);
            }
            if (color_range <= 0 || color_range >= 1)
            {
                fprintf(fp, "\n Invalid color range. Please provide a range between 0 and 1.\n");
                fclose(fp);
                exit(1);
            }
            if (angle < 0 || angle > 180)
            {
                fprintf(fp, "\n Please enter an angle between 0 and 180.\n");
                fclose(fp);
                exit(1);
            }
            if (precision <= 0 || precision >= 1)
            {
                fprintf(fp, "\n Invalid Precision. Please provide a number between 0 and 1.\n");
                fclose(fp);
                exit(1);
            }

            fprintf(fp, "Material 1, Material 2, Step in ratio, Angle, Polarization value, Polarization range, Color value, Color range\n");
            fprintf(fp, ",, %d%%, %d, %f, %f, %f, %f\n\n", (int)(precision*100), angle, polarization_1, pola_range, color, color_range);
            fprintf(fp, ",, Ratio of material 1, Ratio of material 2, Size of material 1, Size of material 2, Polarization value, color value\n");

            for (int i = 0; i < MATERIALS_LEN - 1; i++)
            {
                strcpy(material_1, materials[i]);
                for (int j = i+1; j < MATERIALS_LEN; j++)
                {
                    strcpy(material_2, materials[j]);

                    array_file_1_blue = (float **) malloc(SIZES_LEN  * sizeof(float*));
                    array_file_1_red = (float **) malloc(SIZES_LEN  * sizeof(float*));
                    array_file_2_blue = (float **) malloc(SIZES_LEN * sizeof(float*));
                    array_file_2_red = (float **) malloc(SIZES_LEN  * sizeof(float*));

                    for (int row = 0; row<SIZES_LEN; row++)
                    {
                        array_file_1_blue[row] = (float *) malloc((angle + 1) * sizeof(float));
                        array_file_1_red[row] = (float *) malloc((angle + 1) * sizeof(float));
                        array_file_2_blue[row] = (float *) malloc((angle + 1) * sizeof(float));
                        array_file_2_red[row] = (float *) malloc((angle + 1) * sizeof(float));
                    }
                    //Find the size and percent of mixture given two materials and color of mixture.

                    result_1 = find_size_r_two(fp, material_1, material_2, original_path, &current_file_1_blue, &current_file_1_red, &current_file_2_blue, &current_file_2_red);
                    if (!result_1)
                    {
                        fprintf(fp, "\n Problem with find_size_r_two\n");
                    }
                    else
                    {
                        result_2 = find_size_r_two_helper(fp, material_1, material_2, angle, current_file_1_blue, current_file_1_red, current_file_2_blue, current_file_2_red, array_file_1_red, array_file_1_blue, array_file_2_red, array_file_2_blue);
                        if (!result_2)
                        {
                            fprintf(fp, "\n Problem with find_size_r_two_helper\n");
                        }
                        else
                        {
                            result_3 = find_size_r_two_helper_2(fp, material_1, material_2, original_path, polarization_1, pola_range, color, color_range, angle, precision, array_file_1_red, array_file_1_blue, array_file_2_red, array_file_2_blue);
                            if (result_3)
                            {
                                result_4 = 1;
                            }
                        }
                    }

                    for (int row = 0; row<SIZES_LEN; row++)
                    {
                        free(array_file_1_blue[row]);
                        free(array_file_1_red[row]);
                        free(array_file_2_blue[row]);
                        free(array_file_2_red[row]);
                    }
                    free(array_file_1_blue);
                    free(array_file_1_red);
                    free(array_file_2_blue);
                    free(array_file_2_red);
                }
            }
            if(!result_4)
            {
                fprintf(fp, "\n Please try increasing the range (of the polarization and/or the color range) or decreasing the precision value.\n");
            }
        }

        else
        {
            fprintf(fp, "Wrong command, Please try again.\n");
            fclose(fp);
            exit(1);
        }
    }

    else if (argc == 9)
    {
        sscanf(argv[1]," %s", command);

        if (!strcmp(command, "find_pola") || !strcmp(command, "find_bright"))
        {
            sscanf(argv[2]," %s", material_1);
            sscanf(argv[3]," %s", material_2);
            size_1 = atof(argv[4]);
            size_2 = atof(argv[5]);
            angle = atoi(argv[6]);
            sscanf(argv[7]," %s", color_1);
            step = atof(argv[8]);

            if (!strcmp(color_1, "B"))
            {
                strcpy(color_in_full, "Blue");
            }
            else
            {
                strcpy(color_in_full, "Red");
            }

            fprintf(fp, "Filter, Angle, Step size\n %s, %d, %f", color_in_full, angle, step);

            for (i = 0; i < MATERIALS_LEN; i++)
            {
                if (!strcmp(materials[i], material_1))
                {
                    material_check = 1;
                }
                if (!strcmp(materials[i], material_2))
                {
                    material_check_2 = 1;
                }
            }
            if (!material_check || !material_check_2)
            {
                fprintf(fp, "\n\n Wrong material. Please enter a valid material.\n");
                fclose(fp);
                exit(1);
            }

            if (angle < 0 || angle > 180)
            {
                fprintf(fp, "\n\n Please enter an angle between 0 and 180.\n");
                fclose(fp);
                exit(1);
            }
            if (strcmp("B", color_1) && strcmp("R", color_1))
            {
                fprintf(fp, "\n\n Invalid filter. Please enter 'B' for blue and 'R' for red.\n");
                fclose(fp);
                exit(1);
            }
            if(size_1 <= 0 || size_2 <= 0)
            {
                fprintf(fp, "\n\n Invalid size. Please enter a positive size.\n");
                fclose(fp);
                exit(1);
            }
            if(step <= 0 || step >= 1)
            {
                fprintf(fp, "\n\n Invalid step size. Please enter a value strictly between 0 and 1.\n");
                fclose(fp);
                exit(1);
            }
            result_1 = find_polarization(fp, &polarization_1, &brightness_1, material_1, color_1, size_1, angle, original_path);
            result_2 = find_polarization(fp, &polarization_2, &brightness_2, material_2, color_1, size_2, angle, original_path);

            if (result_1 == 1 && result_2 == 1)
            {
                fprintf(fp, "\n\n Ratio of %s, Ratio of %s, Polarization\n", material_1, material_2);
                for (j = 0; j < 1; j+=step)
                {
                    //Given the polarization and brightness of both materials, find the total brightness.
                    s11 = (j * brightness_1) + ((1-j) * brightness_2);
                    if (s11 != 0)
                    {
                        result_3 = 1;
                        total_polarization = ((j * polarization_1 * brightness_1) + ((1 - j) * polarization_2 * brightness_2))/(s11);
                        fprintf(fp, "%d%%, %d%%, %f\n", (int)(j * 100),(int)((1-j) * 100), total_polarization);
                    }
                }
            }
            if (!result_3)
            {
                fprintf(fp, "No data found, Please try decreasing the range.\n");
            }
        }
        else
        {
            fprintf(fp, "\n\n Wrong command, Please try again.\n");
            fclose(fp);
            exit(1);
        }
    }
    else
    {
        fprintf(fp, "Wrong number of values, Please try again.\n");
        fclose(fp);
        exit(1);
    }
    fclose(fp);

}

int  find_polarization(FILE * fp, float* polarization_1, float* brightness_1, const char* material_1, const char* color_1,  float size_1, int angle, char* original_path)
{
    FILE* polarization_file, *brightness_file;
    int k, i, j, int_size = 0;
    float m, sizes[SIZES_LEN + 1] = {0.112939, 0.148184, 0.194428, 0.255104, 0.334716, 0.439172, 0.576226, 0.756051, 0.991995, 1.301570, 1.707756, 2.240701, 2.939965, 3.857451, 5.061259};
    char path[MAX_LEN], path_bright[MAX_LEN];
    char *record;
    float pola_array[SIZES_LEN][ANGLES_LEN], bright_array [SIZES_LEN][ANGLES_LEN];
    char buffer_pola[MAX_SIZE], buffer_bright[MAX_SIZE];

    if (size_1 <= 0 || size_1 >= 6)
    {
        fprintf(fp, "\n\n Error:,Size given to find_polarization is out of range, Please enter a number strictly between 0 and 6.\n");
        return 0;
    }
    strcpy(path, original_path);
    strcat(path, "/");
    strcat(path, material_1);
    strcat(path, "/");
    // copy path into path_bright for later use
    strcpy(path_bright, path);

    //Finds size from sizes array by looking for the closest value to the given size.
    m = fabs(sizes[0] - size_1);
    for (k = 0; k < SIZES_LEN; k++)
    {
        if (fabs(sizes[k] - size_1) < m)
        {
            m = fabs(sizes[k] - size_1);
            int_size = k;
        }
    }
    //If filter is blue, then open blue file and find polarization and brightness of material.
    if (!strcmp(color_1, "B"))
    {
        // concatenate current path with filename
        strcat(path, "pola_0.45.txt");
        polarization_file = fopen(path, "r");
        if (!polarization_file)
        {
            fprintf(fp, "\n\n Polarization file of %s blue filter cannot be opened.\n", material_1);
            fprintf(fp, "path : %s\n", path);
            return 0;
        }
        // concatenate current copied path with filename
        strcat(path_bright, "bright_0.45.txt");
        brightness_file = fopen(path_bright, "r");
        if (!brightness_file)
        {
            fprintf(fp, "\n\n Brightness of %s blue filter file cannot be opened.\n", material_1);
            fprintf(fp, "path : %s\n", path_bright);
            return 0;
        }
    }
    else if (!strcmp(color_1, "R"))
    {
        // concatenate current path with filename
        strcat(path, "pola_0.65.txt");
        polarization_file = fopen(path, "r");
        if (!polarization_file)
        {
            fprintf(fp, "\n\n Polarization file of %s red filter cannot be opened.\n", material_1);
            fprintf(fp, "path : %s\n", path);
            return 0;
        }
        // concatenate current copied path with filename
        strcat(path_bright, "bright_0.65.txt");
        brightness_file = fopen(path_bright, "r");
        if (!brightness_file)
        {
            fprintf(fp, "\n\n Brightness file of %s red filter cannot be opened.\n", material_1);
            fprintf(fp, "path : %s\n", path_bright);
            return 0;
        }
    }
    else
    {
        fprintf(fp, "\n\n Color Error.\n");
        exit(1);
    }

    for (i = 0; i <= int_size; i++)
    {
        if(fgets(buffer_pola, MAX_SIZE, polarization_file))
        {
            record = strtok(buffer_pola,",");
            if (record)
            {
                //Read polarization values into a 2D array until column angle
                for (j = 0; j <= angle; j++)
                {
                    pola_array[i][j] = atof(record);
                    record = strtok(NULL,",");
                }
            }
        }
    }
    fclose(polarization_file);

    for (i = 0; i <= int_size; i++)
    {
        if (fgets(buffer_bright, MAX_SIZE, brightness_file))
        {
            record = strtok(buffer_bright,",");
            if (record)
            {
                //Read brightness values into a 2D array until column angle
                for (j = 0; j <= angle; j++)
                {
                    bright_array[i][j] = atof(record);
                    record = strtok(NULL,",");
                }
            }
        }
    }
    fclose(brightness_file);
    //Dereference the pointers polarization_1 and brightness_1 with the appropriate values.
    *polarization_1 = pola_array[int_size][angle];
    *brightness_1 = bright_array[int_size][angle];

    return 1;
}

//Gives the different possible combination of sizes of the two materials that yield the given color with the given angle.
int find_size_r_two(FILE * fp, char* material_1, char* material_2, char* original_path, FILE ** current_file_1_blue, FILE ** current_file_1_red, FILE ** current_file_2_blue, FILE ** current_file_2_red)
{
    char path[MAX_LEN];
    // Open all four files needed to calculate the color: 2 files for each material.
    strcpy(path, original_path);
    strcat(path, "/");
    strcat(path, material_1);
    strcat(path, "/");
    strcat(path, "bright_0.65.txt");
    *current_file_1_red = fopen(path, "r");
    if (!current_file_1_red)
    {
        fprintf(fp, "%s could not be opened.\n", path);
        return 0;
    }

    strcpy(path, original_path);
    strcat(path, "/");
    strcat(path, material_1);
    strcat(path, "/");
    strcat(path, "bright_0.45.txt");
    *current_file_1_blue = fopen(path, "r");
    if (!current_file_1_blue)
    {
        fprintf(fp, "%s could not be opened.\n", path);
        return 0;
    }

    strcpy(path, original_path);
    strcat(path, "/");
    strcat(path, material_2);
    strcat(path, "/");
    strcat(path, "bright_0.65.txt");
    *current_file_2_red = fopen(path, "r");
    if (!current_file_2_red)
    {
        fprintf(fp, "%s could not be opened.\n", path);
        return 0;
    }

    strcpy(path, original_path);
    strcat(path, "/");
    strcat(path, material_2);
    strcat(path, "/");
    strcat(path, "bright_0.45.txt");
    *current_file_2_blue = fopen(path, "r");
    if (!current_file_2_blue)
    {
        fprintf(fp, "%s could not be opened.\n", path);
        return 0;
    }

    if (current_file_1_blue && current_file_1_red && current_file_2_blue && current_file_2_red)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int find_size_r_two_helper(FILE * fp, char* material_1, char* material_2, int angle, FILE *current_file_1_blue, FILE *current_file_1_red, FILE *current_file_2_blue, FILE *current_file_2_red, float** array_file_1_red, float** array_file_1_blue, float** array_file_2_red, float** array_file_2_blue)
{
    char *line_file_1_blue = NULL, *line_file_1_red = NULL, *line_file_2_blue = NULL, *line_file_2_red = NULL;
    char *record_file_1_blue, *record_file_1_red, *record_file_2_blue, *record_file_2_red;
    char  buffer_file_1_blue[MAX_SIZE], buffer_file_1_red[MAX_SIZE], buffer_file_2_blue[MAX_SIZE], buffer_file_2_red[MAX_SIZE];

    if (current_file_1_blue && current_file_1_red && current_file_2_blue && current_file_2_red)
    {
        //Read values from files to 2D arrays until column angle
        for (int j = 0; j < SIZES_LEN; j++)
        {
            if ((line_file_1_blue = fgets(buffer_file_1_blue, MAX_SIZE, current_file_1_blue)))
            {
                record_file_1_blue = strtok(line_file_1_blue,",");
                if (record_file_1_blue)
                {
                    for (int k = 0; k <= angle; k++)
                    {
                        array_file_1_blue[j][k] = atof(record_file_1_blue);
                        record_file_1_blue = strtok(NULL,",");
                    }
                }
                else
                {
                    fprintf(fp, "Error:,record_file_1_blue of %s is NULL.\n", material_1);
                    return 0;
                }
            }
            else
            {
                fprintf(fp, "Error:,line_file_1_blue of %s is NULL.\n", material_1);
                return 0;
            }
        }
        for (int j = 0; j < SIZES_LEN; j++)
        {
            if((line_file_1_red = fgets(buffer_file_1_red, MAX_SIZE, current_file_1_red)))
            {
                record_file_1_red = strtok(line_file_1_red,",");
                if (record_file_1_red)
                {
                    for (int k = 0; k <= angle; k++)
                    {
                        array_file_1_red[j][k] = atof(record_file_1_red);
                        record_file_1_red = strtok(NULL,",");
                    }
                }
                else
                {
                    fprintf(fp, "Error:,record_file_1_red of %s is NULL.\n", material_1);
                    return 0;
                }
            }
            else
            {
                fprintf(fp, "Error:,line_file_1_red of %s is NULL.\n", material_1);
                return 0;
            }
        }

        for (int j = 0; j < SIZES_LEN; j++)
        {
            if((line_file_2_blue = fgets(buffer_file_2_blue, MAX_SIZE, current_file_2_blue)))
            {
                record_file_2_blue = strtok(line_file_2_blue,",");
                if (record_file_2_blue)
                {
                    for (int k = 0; k <= angle; k++)
                    {
                        array_file_2_blue[j][k] = atof(record_file_2_blue);
                        record_file_2_blue = strtok(NULL,",");
                    }
                }
                else
                {
                    fprintf(fp, "Error:,record_file_2_blue of %s is NULL.\n", material_2);
                    return 0;
                }
            }
            else
            {
                fprintf(fp, "Error:,line_file_2_blue of %s is NULL.\n", material_2);
                return 0;
            }
        }

        for (int j = 0; j < SIZES_LEN; j++)
        {
            if((line_file_2_red = fgets(buffer_file_2_red, MAX_SIZE, current_file_2_red)))
            {
                record_file_2_red = strtok(line_file_2_red,",");
                if(record_file_2_red)
                {
                    for (int k = 0; k <= angle; k++)
                    {
                        array_file_2_red[j][k] = atof(record_file_2_red);
                        record_file_2_red = strtok(NULL,",");
                    }
                }
                else
                {
                    fprintf(fp, "Error:,record_file_2_red of %s is NULL.\n", material_2);
                    return 0;
                }
            }
            else
            {
                fprintf(fp, "Error:,line_file_2_red of %s is NULL.\n", material_2);
                return 0;
            }
        }
    }
    return 1;
}

int find_size_r_two_helper_2(FILE * fp, char* material_1, char* material_2, char* original_path, float polarization, float pola_range, float color, float color_range, int angle, float precision, float** array_file_1_red, float** array_file_1_blue, float** array_file_2_red, float** array_file_2_blue)
{
    float sizes[MAX_LEN] = {0.112939, 0.148184, 0.194428, 0.255104, 0.334716, 0.439172, 0.576226, 0.756051, 0.991995, 1.301570, 1.707756, 2.240701, 2.939965, 3.857451, 5.061259};
    float current_color = 0.0, to_be_loged = 0.0, numerator = 0.0, denominator = 0.0, color_diff = 0.0, r = 0.0;
    float polarization_1 = 0, polarization_2 = 0, brightness_1 = 0, brightness_2 = 0, total_polarization = 0, s11 = 0, pola_diff = 0, mat_1_size = 0, mat_2_size = 0;
    int result = 0, result_1 = 0, result_2 = 0;

    //Work with every element of all 4 2D arrays to find the different possible combinations of sizes,
    //ratios of the two materials that gives the desired color of the mixture with the approximation.
    for (int i = 0; i < SIZES_LEN; i++)
    {
        for (int j = 0; j < SIZES_LEN; j++)
        {
            for (r = 0; r <= 1; r+=precision)
            {
                numerator = (array_file_1_red[i][angle-1] * r) + (array_file_2_red[j][angle-1] * (1 - r));
                denominator = ((array_file_1_blue[i][angle] * r) + (array_file_2_blue[j][angle] * (1 - r)));
                if (denominator!=0)
                {
                    to_be_loged =  numerator / denominator;
                }
                current_color = -2.5 * log10(to_be_loged);
                color_diff = fabs(color) - fabs(current_color);

                if (fabs(color_diff) <= color_range)
                {
                    result_1 = find_polarization(fp, &polarization_1, &brightness_1, material_1, "R", sizes[i], angle, original_path);
                    result_2 = find_polarization(fp, &polarization_2, &brightness_2, material_2, "R", sizes[j], angle, original_path);
                    if (result_1 && result_2)
                    {

                        //Given the polarization and brightness of both materials, find the total brightness.
                        s11 = (r * brightness_1) + ((1-r) * brightness_2);
                        if (s11 != 0)
                        {
                            total_polarization = ((r * polarization_1 * brightness_1) + ((1 - r) * polarization_2 * brightness_2))/(s11);
                            pola_diff = fabs(polarization) - fabs(total_polarization);
                            if (fabs(pola_diff) <= pola_range)
                            {
                                result = 1; //There is at least one output
                                if(r == 0)
                                {
                                    mat_1_size = 0;
                                    mat_2_size = sizes[j];
                                }
                                else if(r == 1)
                                {
                                    mat_1_size = sizes[i];
                                    mat_2_size = 0;
                                }
                                else
                                {
                                    mat_1_size = sizes[i];
                                    mat_2_size = sizes[j];
                                }
                                fprintf(fp, "%s, %s, %f, %f, %f, %f, %f, %f\n", material_1, material_2, r, (1 - r), mat_1_size, mat_2_size, total_polarization, current_color);
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}
