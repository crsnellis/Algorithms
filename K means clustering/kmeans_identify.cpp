#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

int minfunc(double a, double b, double c)
{
    if(a < b && a < c){
        return 1;
    }
    else if(b < a && b < c){
        return 2;
    }
    else if(c < a && c < b){
        return 3;
    }
    else 
        return 1;
}

struct car_data
{
    std::vector<std::string> headers; // stores the headers

    int N_rows; // stores number of rows

    // data in the 8 columns in cars.csv
    std::vector<double> mpg;
    std::vector<double> cylinders;
    std::vector<double> cubicinches;
    std::vector<double> hp;
    std::vector<double> weightlbs;
    std::vector<double> time260;
    std::vector<double> year;
    std::vector<std::string> region;

    std::vector<int> clusters; // cluster 1,2,3 that car is assigned to
};

void read_csv(std::ifstream &fin, car_data &dat)
{
    std::string temp1, reg_temp, reg;
    double m, cyl, cubic, hp, lbs, to60, year;
    int i = 0;
    while(fin >> temp1 >> reg_temp)
    {
        //account for first row
        if(i < 4){
            dat.headers.push_back(temp1.substr(0, temp1.size() - 1));
            if(i == 3)
                dat.headers.push_back("region");
            else if(i != 3)
                dat.headers.push_back(reg_temp.substr(0, reg_temp.size() - 1));
            i++;
            continue;
        }
        else if(i > 3){
        //REGION
        reg = reg_temp.substr(0, reg_temp.size() - 1);
        
        //populate array to find the commas in the gathered string
        int arr[7]; 
        int j = 0;
        for(int k = 0; k < temp1.size(); k++)
        {
            if(temp1.at(k) == ',')
            {
                arr[j] = k;
                j++;
            }
        }

        //create double values and populate the struct
        m = stod(temp1.substr(0, arr[0]));
        cyl = stod(temp1.substr(arr[0] + 1, arr[1] - arr[0] - 1)); 
        cubic = stod(temp1.substr(arr[1] + 1, arr[2] - arr[1] - 1));
        hp = stod(temp1.substr(arr[2] + 1, arr[3] - arr[2] - 1));
        lbs = stod(temp1.substr(arr[3] + 1, arr[4] - arr[3] - 1));
        to60 = stod(temp1.substr(arr[4] + 1, arr[5] - arr[4] - 1));
        year = stod(temp1.substr(arr[5] + 1, arr[6] - arr[5] - 1));
        //cout << m << " " << cyl << " " << cubic << " " << hp << " " << lbs << " " << to60 << " " << year << endl;
        dat.mpg.push_back(m);
        dat.cylinders.push_back(cyl);
        dat.cubicinches.push_back(cubic);
        dat.hp.push_back(hp);
        dat.weightlbs.push_back(lbs);
        dat.time260.push_back(to60);
        dat.year.push_back(year);
        dat.region.push_back(reg);
        dat.N_rows = i - 3;
        i++;
    }
    }
}

int main()
{
    double a, b, c, d, e, f, g;
    //cout << "Enter mystery car values MAKE RIGHT" << endl;
    std::cin >> a >> b >> c >> d >> e >> f >> g;

    std::ifstream fin("clusters.csv");
    std::string temp;
    double c1[8], c2[8], c3[8];
    int i = 0, centroid_num;
    //ASSIGN VALUES TO CENTROID ARRAYS
    while(fin >> temp)
    {
        if(i > 7 && i < 15)
        {
        c1[i - 7] = stod(temp.substr(0,temp.size() - 1));
        }
        else if(i > 15 && i < 23)
        {
            c2[i - 16] = stod(temp.substr(0,temp.size() - 1));
        }
        else if(i > 23 && i < 31)
        {
            c3[i - 24] = stod(temp.substr(0,temp.size() - 1));
        }
        i++;
    }
    fin.close();

    //MPG SECTION ------------------------------------
    //find the minimum and maximum for MPG
    int m = 0;
    int j = 1;
    fin.open("cars.csv");
    car_data data;
    read_csv(fin, data);
    fin.close();
    double min1, min2, min3, min4, min5, min6, min7, max1, max2, max3, max4, max5, max6, max7;
    while(j < data.mpg.size() + 1)
    {
        if(m == 0)
       {
           min1 = data.mpg[m];
           max1 = min1;
           min2 = data.cylinders[m];
           max2 = min2;
           min3 = data.cubicinches[m];
           max3 = min3;
           min4 = data.hp[m];
           max4 = min4;
           min5 = data.weightlbs[m];
           max5 = min5;
           min6 = data.time260[m];
           max6 = min6;
           min7 = data.year[m];
           max7 = min7;
       }
 
       if(j%8 == 1){
       if(data.mpg[m] < min1)
           min1 = data.mpg[m];
       else if(data.mpg[m] > max1)
           max1 = data.mpg[m];
       }
       if(j%8 == 2){
       if(data.cylinders[m] < min2)
           min2 = data.cylinders[m];
       else if(data.cylinders[m] > max2)
           max2 = data.cylinders[m];
       }
       if(j%8 == 3){
       if(data.cubicinches[m] < min3)
           min3 = data.cubicinches[m];
       else if(data.cubicinches[m] > max3)
           max3 = data.cubicinches[m];
       }
       if(j%8 == 4){
       if(data.hp[m] < min4)
           min4 = data.hp[m];
       else if(data.hp[m] > max4)
           max4 = data.hp[m];
       }
       if(j%8 == 5){
       if(data.weightlbs[m] < min5)
           min5 = data.weightlbs[m];
       else if(data.weightlbs[m] > max5)
           max5 = data.weightlbs[m];
       }
       if(j%8 == 6){
       if(data.time260[m] < min6)
           min6 = data.time260[m];
       else if(data.time260[m] > max6)
           max6 = data.time260[m];
       }
       if(j%8 == 7){
       if(data.year[m] < min7)
           min7 = data.year[m];
       else if(data.year[m] > max7)
           max7 = data.year[m];
           m++;
       }
       j++;
   }


    a = (a-min1)/(max1-min1);
    b = (b-min2)/(max2-min2);
    c = (c-min3)/(max3-min3);
    d = (d-min4)/(max4-min4);
    e = (e-min5)/(max5-min5);
    f = (f-min6)/(max6-min6);
    g = (g-min7)/(max7-min7);
    
    double minarr[] = {min1, min2, min3, min4, min5, min6, min7};
    double maxarr[] = {max1, max2, max3, max4, max5, max6, max7};
    int u = 0;
    while(u < 7){
        c1[u] = (c1[u] - minarr[u])/(maxarr[u]-minarr[u]);
        c2[u] = (c2[u] - minarr[u])/(maxarr[u]-minarr[u]);
        c3[u] = (c3[u] - minarr[u])/(maxarr[u]-minarr[u]);
        u++;
    }

    //DO CALCULATION
    double d1, d2, d3;
    d1 = sqrt(pow(a - c1[0], 2)
                            + pow(b - c1[1], 2)
                            + pow(c - c1[2], 2)
                            + pow(d - c1[3], 2)
                            + pow(e - c1[4], 2)
                            + pow(f - c1[5], 2)
                            + pow(g - c1[6], 2));
    d2 = sqrt(pow(a - c2[0], 2)
                            + pow(b - c2[1], 2)
                            + pow(c - c2[2], 2)
                            + pow(d - c2[3], 2)
                            + pow(e - c2[4], 2)
                            + pow(f - c2[5], 2)
                            + pow(g - c2[6], 2));
    d3 = sqrt(pow(a - c3[0], 2)
                            + pow(b - c3[1], 2)
                            + pow(c - c3[2], 2)
                            + pow(d - c3[3], 2)
                            + pow(e - c3[4], 2)
                            + pow(f - c3[5], 2)
                            + pow(g - c3[6], 2));
    centroid_num = minfunc(d1, d2, d3);
    std::cout << "Mystery car is type " << centroid_num << ".";
}