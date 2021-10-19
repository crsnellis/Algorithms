#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
// NOTE: before optimization, 625 lines

int minfunc(double a, double b, double c)
{
    if((a < b && a < c) || a == 0){
        return 1;
    }
    else if((b < a && b < c) || b == 0){
        return 2;
    }
    else{
        return 3;
    }
    
}
// structure for holding data from cars.csv
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

void read_csv(std::ifstream &fin, car_data &dat); // read data from cars.csv into struct car_data
void normalize(car_data &data_norm); // normalize data
void unnormalize(car_data &data_norm,car_data &data); // take normalized data_norm and unnormalize it using max/min values from data
void initialize(car_data &centroids, car_data &data_norm); // give random initial values for centroids
double check_convergence(const car_data &centroids, const car_data &centroids_old); // find sum of squares difference between old and new centroids
void assign_clusters(const car_data &centroids, car_data &data_norm); // for each mpg, etc. find nearest cluster
void find_new_centroids(car_data &centroids, const car_data &data_norm); // find average position for each cluster to define new centroids
void print(std::ostream &fout, const car_data &data); // print to cout or file (ostream can be either)

int main()
{
    car_data data;
    std::ifstream fin("cars.csv");
    // Step 1 - read in data and verify
    read_csv(fin, data);
    data.headers.push_back("cluster");
    // print(cout,data); // good to check output for debugging
    fin.close();

    // Step 2 - Normalize std::vectors - only columns 1-7
    car_data data_norm = data;
    normalize(data_norm);
    // print(cout,data_norm); // good to check output for debugging

    // Step 3 - Initialize centroids
    car_data centroids;
    initialize(centroids, data_norm);
    centroids.headers = data.headers;
    // print(cout,centroids); // good to check output for debugging

    // Step 4 - Now assign points to cluster
    car_data centroids_old = centroids; // difference between this iteration and last
    const double EPSILON = 1.0e-10; // tolerance for convergence criterion
    bool first_step = true;         // this is needed so initially difference between new & old is not 0
    while ((check_convergence(centroids, centroids_old) > EPSILON) || first_step)
    {
        first_step = false;

        centroids_old = centroids;              // store previous centroids           
        assign_clusters(centroids, data_norm);    // set cluster allocation by nearest neighbour
        find_new_centroids(centroids, data_norm); // calculate new centroids by mean of clusters
        // print(cout, centroids); // good to check output for debugging
    }

    // Step 5 - print out clustered data and centroids
    data.clusters = data_norm.clusters;
    std::ofstream fout("cars_clustered.csv");
    print(fout, data);
    fout.close();
    fout.open("clusters.csv");
    unnormalize(centroids,data);
    print(fout, centroids);
    fout.close();
}

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

void normalize(car_data &data_norm)
{
    double min, max;
    //MPG SECTION ------------------------------------

    for(int i = 0; i < data_norm.mpg.size(); i++)
    {
        if(i == 0)
        {
            min = data_norm.mpg[i];
            max = min;
        }
        if(data_norm.mpg[i] < min)
            min = data_norm.mpg[i];
        else if(data_norm.mpg[i] > max)
            max = data_norm.mpg[i];
    }

    for(int i = 0; i < data_norm.mpg.size(); i++)
    {
        data_norm.mpg[i] = (data_norm.mpg[i] - min)/(max-min);
    }
    //CYLINDER SECTION ---------------------------------
    for(int i = 0; i < data_norm.cylinders.size(); i++)
   {
       if(i == 0)
       {
           min = data_norm.cylinders[i];
           max = min;
       }
       if(data_norm.cylinders[i] < min)
           min = data_norm.cylinders[i];
       else if(data_norm.cylinders[i] > max)
           max = data_norm.cylinders[i];
   }
 
   for(int i = 0; i < data_norm.cylinders.size(); i++)
   {
       data_norm.cylinders[i] = (data_norm.cylinders[i] - min)/(max-min);
   }

    //CUBICINCHES
    for(int i = 0; i < data_norm.cubicinches.size(); i++)
   {
       if(i == 0)
       {
           min = data_norm.cubicinches[i];
           max = min;
       }
       if(data_norm.cubicinches[i] < min)
           min = data_norm.cubicinches[i];
       else if(data_norm.cubicinches[i] > max)
           max = data_norm.cubicinches[i];
   }
 
   for(int i = 0; i < data_norm.cubicinches.size(); i++)
   {
       data_norm.cubicinches[i] = (data_norm.cubicinches[i] - min)/(max-min);
   }

    //HP
    for(int i = 0; i < data_norm.hp.size(); i++)
   {
       if(i == 0)
       {
           min = data_norm.hp[i];
           max = min;
       }
       if(data_norm.hp[i] < min)
           min = data_norm.hp[i];
       else if(data_norm.hp[i] > max)
           max = data_norm.hp[i];
   }
 
   for(int i = 0; i < data_norm.hp.size(); i++)
   {
       data_norm.hp[i] = (data_norm.hp[i] - min)/(max-min);
   }

    //WEIGHTLBS
    for(int i = 0; i < data_norm.weightlbs.size(); i++)
   {
       if(i == 0)
       {
           min = data_norm.weightlbs[i];
           max = min;
       }
       if(data_norm.weightlbs[i] < min)
           min = data_norm.weightlbs[i];
       else if(data_norm.weightlbs[i] > max)
           max = data_norm.weightlbs[i];
   }
 
   for(int i = 0; i < data_norm.weightlbs.size(); i++)
   {
       data_norm.weightlbs[i] = (data_norm.weightlbs[i] - min)/(max-min);
   }

    //TIME260
    for(int i = 0; i < data_norm.time260.size(); i++)
   {
       if(i == 0)
       {
           min = data_norm.time260[i];
           max = min;
       }
       if(data_norm.time260[i] < min)
           min = data_norm.time260[i];
       else if(data_norm.time260[i] > max)
           max = data_norm.time260[i];
   }
 
   for(int i = 0; i < data_norm.time260.size(); i++)
   {
       data_norm.time260[i] = (data_norm.time260[i] - min)/(max-min);
   }

    //YEAR
    for(int i = 0; i < data_norm.year.size(); i++)
   {
       if(i == 0)
       {
           min = data_norm.year[i];
           max = min;
       }
       if(data_norm.year[i] < min)
           min = data_norm.year[i];
       else if(data_norm.year[i] > max)
           max = data_norm.year[i];
   }
 
   for(int i = 0; i < data_norm.year.size(); i++)
   {
       data_norm.year[i] = (data_norm.year[i] - min)/(max-min);
   }
}

void unnormalize(car_data &data_norm,car_data &data)
{
    double min, max;
   //MPG SECTION ------------------------------------
   //find the minimum and maximum for MPG
   for(int i = 0; i < data.mpg.size(); i++)
   {
       if(i == 0)
       {
           min = data.mpg[i];
           max = min;
       }
       if(data.mpg[i] < min)
           min = data.mpg[i];
       else if(data.mpg[i] > max)
           max = data.mpg[i];
   }
 
   for(int i = 0; i < data_norm.mpg.size(); i++)
   {
       data_norm.mpg[i] = min + (data_norm.mpg[i])*(max-min);
   }
   //CYLINDER SECTION ---------------------------------
   for(int i = 0; i < data.cylinders.size(); i++)
  {
      if(i == 0)
      {
          min = data.cylinders[i];
          max = min;
      }
      if(data.cylinders[i] < min)
          min = data.cylinders[i];
      else if(data.cylinders[i] > max)
          max = data.cylinders[i];
  }
  for(int i = 0; i < data_norm.cylinders.size(); i++)
   {
       data_norm.cylinders[i] = min + (data_norm.cylinders[i])*(max-min);
   }
 
   //CUBICINCHES
   for(int i = 0; i < data.cubicinches.size(); i++)
  {
      if(i == 0)
      {
          min = data.cubicinches[i];
          max = min;
      }
      if(data.cubicinches[i] < min)
          min = data.cubicinches[i];
      else if(data.cubicinches[i] > max)
          max = data.cubicinches[i];
  }
  for(int i = 0; i < data_norm.cubicinches.size(); i++)
   {
       data_norm.cubicinches[i] = min + (data_norm.cubicinches[i])*(max-min);
   }
 
   //HP
   for(int i = 0; i < data.hp.size(); i++)
  {
      if(i == 0)
      {
          min = data.hp[i];
          max = min;
      }
      if(data.hp[i] < min)
          min = data.hp[i];
      else if(data.hp[i] > max)
          max = data.hp[i];
  }
  for(int i = 0; i < data_norm.hp.size(); i++)
   {
       data_norm.hp[i] = min + (data_norm.hp[i])*(max-min);
   }
 
   //WEIGHTLBS
   for(int i = 0; i < data.weightlbs.size(); i++)
  {
      if(i == 0)
      {
          min = data.weightlbs[i];
          max = min;
      }
      if(data.weightlbs[i] < min)
          min = data.weightlbs[i];
      else if(data.weightlbs[i] > max)
          max = data.weightlbs[i];
  }
  for(int i = 0; i < data_norm.weightlbs.size(); i++)
   {
       data_norm.weightlbs[i] = min + (data_norm.weightlbs[i])*(max-min);
   }
 
   //TIME260
   for(int i = 0; i < data.time260.size(); i++)
  {
      if(i == 0)
      {
          min = data.time260[i];
          max = min;
      }
      if(data.time260[i] < min)
          min = data.time260[i];
      else if(data.time260[i] > max)
          max = data.time260[i];
  }
  for(int i = 0; i < data_norm.time260.size(); i++)
   {
       data_norm.time260[i] = min + (data_norm.time260[i])*(max-min);
   }
 
   //YEAR
   for(int i = 0; i < data.year.size(); i++)
  {
      if(i == 0)
      {
          min = data.year[i];
          max = min;
      }
      if(data.year[i] < min)
          min = data.year[i];
      else if(data.year[i] > max)
          max = data.year[i];
  }
  for(int i = 0; i < data_norm.year.size(); i++)
   {
       data_norm.year[i] = min + ((data_norm).year[i])*(max-min);
   }
}

void initialize(car_data &centroids, car_data &data_norm)// give random initial values for centroids
{
    srand(2);
    std::vector<double> empvec(3);
    std::vector<int> empvec2(3);
    centroids.mpg = empvec;
    centroids.cylinders = empvec;
    centroids.cubicinches = empvec;
    centroids.hp = empvec;
    centroids.weightlbs = empvec;
    centroids.time260 = empvec;
    centroids.year = empvec;
    centroids.clusters = empvec2;
    for(int i = 0; i < 3; i++)
    {
        centroids.mpg.at(i) = data_norm.mpg.at(i);
        centroids.cylinders.at(i) = data_norm.cylinders.at(i);
        centroids.cubicinches.at(i) = data_norm.cubicinches.at(i);
        centroids.hp.at(i) = data_norm.hp.at(i);
        centroids.weightlbs.at(i) = centroids.weightlbs.at(i);
        centroids.time260.at(i) = data_norm.time260.at(i);
        centroids.year.at(i) = data_norm.year.at(i);
        centroids.clusters.at(i) = i + 1;
    }
    centroids.N_rows = 3;
} 

void assign_clusters(const car_data &centroids, car_data &data_norm)
{
        double C1, C2, C3;
        int dR = 0;
        std::vector<int> empvec(data_norm.N_rows);
        data_norm.clusters = empvec;
        while(dR < data_norm.N_rows)
        {
            C1 = sqrt(pow((data_norm.mpg[dR] - centroids.mpg[0]), 2)
                            + pow((data_norm.cylinders[dR] - centroids.cylinders[0]), 2)
                            + pow((data_norm.cubicinches[dR] - centroids.cubicinches[0]), 2)
                            + pow((data_norm.hp[dR] - centroids.hp[0]), 2)
                            + pow((data_norm.weightlbs[dR] - centroids.weightlbs[0]), 2)
                            + pow((data_norm.time260[dR] - centroids.time260[0]), 2)
                            + pow((data_norm.year[dR] - centroids.year[0]), 2));
            C2 = sqrt(pow((data_norm.mpg[dR] - centroids.mpg[1]), 2)
                            + pow((data_norm.cylinders[dR] - centroids.cylinders[1]), 2)
                            + pow((data_norm.cubicinches[dR] - centroids.cubicinches[1]), 2)
                            + pow((data_norm.hp[dR] - centroids.hp[1]), 2)
                            + pow((data_norm.weightlbs[dR] - centroids.weightlbs[1]), 2)
                            + pow((data_norm.time260[dR] - centroids.time260[1]), 2)
                            + pow((data_norm.year[dR] - centroids.year[1]), 2));
            C3 = sqrt(pow((data_norm.mpg[dR] - centroids.mpg[2]), 2)
                            + pow((data_norm.cylinders[dR] - centroids.cylinders[2]), 2)
                            + pow((data_norm.cubicinches[dR] - centroids.cubicinches[2]), 2)
                            + pow((data_norm.hp[dR] - centroids.hp[2]), 2)
                            + pow((data_norm.weightlbs[dR] - centroids.weightlbs[2]), 2)
                            + pow((data_norm.time260[dR] - centroids.time260[2]), 2)
                            + pow((data_norm.year[dR] - centroids.year[2]), 2));
            data_norm.clusters[dR] = minfunc(C1, C2, C3);
            dR++;
        }
} // for each mpg, etc. find nearest cluster

void find_new_centroids(car_data &centroids, const car_data &data_norm) //THE PROBLEM
{
    std::vector<double> empvec(3);
    centroids.mpg = empvec;
    centroids.cylinders = empvec;
    centroids.cubicinches = empvec;
    centroids.hp = empvec;
    centroids.weightlbs = empvec;
    centroids.time260 = empvec;
    centroids.year = empvec;

    int i = 0, x = 0, y = 0, z = 0;
    double mpg1 = 0, cyl1 = 0, cub1 = 0, hp1 = 0, lbs1 = 0, tim1 = 0, year1 = 0, mpg2 = 0, cyl2 = 0, cub2 = 0, hp2 = 0, lbs2 = 0, tim2 = 0, year2 = 0, mpg3 = 0, cyl3 = 0, cub3 = 0, hp3 = 0, lbs3 = 0, tim3 = 0, year3 = 0;
    while(i < data_norm.N_rows)
    {
        if(data_norm.clusters[i] == 1)
        {
            mpg1 += data_norm.mpg[i];
            cyl1 += data_norm.cylinders[i];
            cub1 += data_norm.cubicinches[i];
            hp1 += data_norm.hp[i];
            lbs1 += data_norm.weightlbs[i];
            tim1 += data_norm.time260[i];
            year1 += data_norm.year[i];
            x++;
        }
        if(data_norm.clusters[i] == 2)
        {
            mpg2 += data_norm.mpg[i];
            cyl2 += data_norm.cylinders[i];
            cub2 += data_norm.cubicinches[i];
            hp2 += data_norm.hp[i];
            lbs2 += data_norm.weightlbs[i];
            tim2 += data_norm.time260[i];
            year2 += data_norm.year[i];
            y++;
        }
        if(data_norm.clusters[i] == 3)
        {
            mpg3 += data_norm.mpg[i];
            cyl3 += data_norm.cylinders[i];
            cub3 += data_norm.cubicinches[i];
            hp3 += data_norm.hp[i];
            lbs3 += data_norm.weightlbs[i];
            tim3 += data_norm.time260[i];
            year3 += data_norm.year[i];
            z++;
        }
        i++;
    }
    centroids.mpg[0] = mpg1/x;
    centroids.cylinders[0] = cyl1/x;
    centroids.cubicinches[0] = cub1/x;
    centroids.hp[0] = hp1/x;
    centroids.weightlbs[0] = lbs1/x;
    centroids.time260[0] = tim1/x;
    centroids.year[0] = year1/x;
    centroids.mpg[1] = mpg2/y;
    centroids.cylinders[1] = cyl2/y;
    centroids.cubicinches[1] = cub2/y;
    centroids.hp[1] = hp2/y;
    centroids.weightlbs[1] = lbs2/y;
    centroids.time260[1] = tim2/y;
    centroids.year[1] = year2/y;
    if(z != 0){
    centroids.mpg[2] = mpg3/z;
    centroids.cylinders[2] = cyl3/z;
    centroids.cubicinches[2] = cub3/z;
    centroids.hp[2] = hp3/z;
    centroids.weightlbs[2] = lbs3/z;
    centroids.time260[2] = tim3/z;
    centroids.year[2] = year3/z;
    }
    else{
        centroids.mpg[2] = mpg3;
        centroids.cylinders[2] = cyl3;
        centroids.cubicinches[2] = cub3;
        centroids.hp[2] = hp3;
        centroids.weightlbs[2] = lbs3;
        centroids.time260[2] = tim3;
        centroids.year[2] = year3;
    }
} // find average position for each cluster to define new centroids

double check_convergence(const car_data &centroids, const car_data &centroids_old)
{
    //p("called");
    //const double EPSILON = 1.0e-10; 
    int i = 0;
    double R1 = 0;//, R2 = 0, R3 = 0;
    while(i < 3)
    {
        R1 +=  sqrt(pow((centroids.mpg[i] - centroids_old.mpg[i]), 2)
            + pow((centroids.cylinders[i] - centroids_old.cylinders[i]), 2)
            + pow((centroids.cubicinches[i] - centroids_old.cubicinches[i]), 2)
            + pow((centroids.hp[i] - centroids_old.hp[i]), 2)
            + pow((centroids.weightlbs[i] - centroids_old.weightlbs[i]), 2)
            + pow((centroids.time260[i] - centroids_old.time260[i]), 2)
            + pow((centroids.year[i] - centroids_old.year[i]), 2));
        i++;
    }  
    return R1;
}

void print(std::ostream &fout, const car_data &data)
{
    bool t;
    int i = 0, k;
    double a, b, c, d, e, f, g;
    std::string h;
    fout << "mpg, cylinders, cubicinches, hp, weightlbs, time-to-60, year, cluster" << std::endl;
    if(data.N_rows > 4)
        t = true;
    else
        t = false;
    
    while(i < data.N_rows)
    {
        a = data.mpg[i];
        b = data.cylinders[i];
        c = data.cubicinches[i];
        d = data.hp[i];
        e = data.weightlbs[i];
        f = data.time260[i];
        g = data.year[i];
        k = data.clusters[i];
        
        if(t)
            fout << a << "," << b << "," << c << "," << d << "," << e << "," << f << "," << g << ", " << k << "\n";
        else if(!t)
            fout << a << ", " << b << ", " << c << ", " << d << ", " << e << ", " << f << ", " << g << ", " << k << "\n";
        i++;
    }
}
