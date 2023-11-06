#ifndef RPLOT_H
#define RPLOT_H

#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "Utility.h"
#include <assert.h>

class Yseries {
    public:
        vector<double> D; // data vector
        string C; // color
        string P; // pch == point character
        string T; // type, e.g. point, line, both

        Yseries( vector<double> d, string c, string p, string t ) { D = d; C = c; P = p; T = t; }
        ~Yseries() {};

        int size() { return D.size(); }
        vector<double> data() { return D; }
        double data(int i) { return D[i]; }
        string col() { return C; } 
        string pch() { return P; }
        string type() { return T; }
};


class RPlot {
    private:
        map<string, string> plot_par_str;
        map<string, double> plot_par_num;
        vector<double> X;
        vector<Yseries*> Y;
        string header;
        string footer;

    public:
        RPlot() {};
        ~RPlot() {};
    
        void define_header(string h) { header = h; }
        void define_footer(string f) { footer = f; }

        void define(string par, double val) { 
            if (par == "pch" or par == "col") {
                cerr << "'pch' and 'col' parameters can only be defined in association with a Y data series.  Call add_y(vector<double> y, string color, string pch) instead.\n";
                return;
            }
            plot_par_num[par] = val;
        }

        void define(string par, string val) { 
            if (par == "pch" or par == "col") {
                cerr << "'pch' and 'col' parameters can only be defined in association with a Y data series.  Call add_y(vector<double> y, string color, string pch) instead.\n";
                return;
            }
            plot_par_str[par] = val;
        }

        void set_x(vector<double> x) { 
            if ( Y.size() > 0 and Y[0]->size() > 0 and x.size() != Y[0]->size() ) {
                cerr << "Cannot define a vector of X values with a different length from the Y values\n";
                return;
            }
            X = x;
        }
        
        void set_y(vector<double> y, string color="1", string pch="1", string type="p") { Y.clear(); add_y(y, color, pch, type); } 

        void add_y(vector<double> y, string color="1", string pch="1", string type="p") { 
            if ( X.size() > 0 and X.size() != y.size() ) {
                cerr << "Cannot define a vector of Y values with a different length from the X values\n";
                return;
            }
            Yseries* y_new = new Yseries(y, color, pch, type);
            Y.push_back(y_new);
        }

        int pdf(string filename, double width=10, double height=7.5) {
            _plotter("pdf", filename, width, height);
        }

        int png(string filename, double width=1000, double height=750) {
            _plotter("png", filename, width, height);
        }

        int _plotter(string plot_type, string filename, double width, double height) {
            cerr << "Writing data to file ... ";
            write_datafile("temp.data");
            cerr << "done.\nPreparing R script ... ";

            // write R script
            ofstream Rfile;
            Rfile.open("temp.R");
            Rfile << "d <- read.table(\"temp.data\")\n";
            
            Rfile << plot_type << "(\"" << filename << "\", width=" << width << ", height=" << height << ")\n";

            string y_str = "";
            if (X.size() > 0 and Y.size() > 0) y_str = "d$V2, ";
            
            if (plot_type=="png") { Rfile << "par(mar=c(5, 5, 6, 2) + 0.1)\n"; }

            Rfile << "plot(d$V1, " << y_str;

            // determine and write xlim and ylim, unless defined by user
            if (plot_par_str.count("xlim") == 0) { Rfile << "xlim=" << xlim(X,Y); }
            if (plot_par_str.count("ylim") == 0) { Rfile << "ylim=" << ylim(X,Y); }

            // write out parameters
            map<string,string>::iterator it;
            map<string,double>::iterator itn;
            for ( it=plot_par_str.begin() ; it != plot_par_str.end(); it++ ) Rfile << (*it).first << "=\"" << (*it).second << "\", ";
            for ( itn=plot_par_num.begin() ; itn != plot_par_num.end(); itn++ ) Rfile << (*itn).first << "=" << (*itn).second << ", ";

            if (plot_type=="png") { Rfile << "cex.main=2, cex.axis=2, cex.lab=2, cex.sub=2, "; }
            
            Rfile << "col=\"" << Y[0]->col() << "\", pch=" << Y[0]->pch() << ", type=\"" << Y[0]->type() << "\")\n";

            if (Y.size() > 1) {
                string x_str = "";
                
                int x_ct = 0;
                if (X.size() > 0) {
                    x_str = "d$V1, ";
                    x_ct = 1;
                } 
                for ( unsigned int i = 1; i < Y.size(); i++ ) {
                    stringstream ss;
                    ss << "d$V" << (i+1+x_ct) << ", ";
                    y_str = ss.str();

                    Rfile << "points(" << x_str << y_str;

                    // write out parameters
                    //for ( it=plot_par_str.begin() ; it != plot_par_str.end(); it++ ) Rfile << (*it).first << "=\"" << (*it).second << "\", ";
                    //for ( itn=plot_par_num.begin() ; itn != plot_par_num.end(); itn++ ) Rfile << (*itn).first << "=" << (*itn).second << ", ";
                    
                    Rfile << "col=\"" << Y[i]->col() << "\", pch=" << Y[i]->pch() << ", type=\"" << Y[i]->type() << "\")\n";
                }
            }

            Rfile.close();
            cerr << "done.\nExecuting R script ... ";
            int retval = system( "R CMD BATCH temp.R" );
            cerr << "done.\n";
            return retval; 

        }
        
        void write_datafile(string filename) {
            ofstream datafile;
            datafile.open(filename.c_str());

            int X_len = X.size();
            int longest_Y = 0;
            int series_len = 0;
            for (unsigned int i = 0; i < Y.size(); i++) { 
                longest_Y = max(longest_Y, Y[i]->size());
            }
            series_len = max(X_len, longest_Y);

            for (int i = 0; i < series_len; i++) {
                if (X_len > 0) {
                    if (i >= X_len) {
                        datafile << "NA ";
                    } else {
                        datafile << X[i];
                    }
                }
                for (unsigned int j = 0; j < Y.size(); j++) { 
                    if (i >= Y[j]->size()) {
                        datafile << "NA ";
                    } else {
                        datafile << Y[j]->data(i) << " ";
                    }
                }
                datafile << endl;
            }
            datafile.close();
        }

        string xlim(vector<double> X, vector< Yseries* > Y) {
            string lim_str;

            if (X.size() == 0 and Y.size() == 0) {
                cerr << "Cannot calculate y-limits if no data is defined\n";
                return lim_str;
            }
            
            vector<double> lim(2);
            if (X.size() > 0) {
                lim[0] = min_element(X);
                lim[1] = max_element(X);
            } else {
                lim[0] = 1;
                lim[1] = 0;
                for (unsigned int i=0; i<Y.size(); i++) {
                    if (Y[i]->size() > lim[1]) lim[1] = Y[i]->size();
                }
            }
            
            stringstream ss;
            ss << "c(" << lim[0] << "," << lim[1] << "), ";
            lim_str = ss.str(); 
            return lim_str;
        }

        string ylim(vector<double> X, vector< Yseries* > Y) {
            string lim_str;
            
            if (X.size() == 0 and Y.size() == 0) {
                cerr << "Cannot calculate y-limits if no data is defined\n";
                return lim_str;
            }
            
            vector<double> lim(2);
            if (Y.size() > 0) {
                lim = determine_limits( Y );
            } else {
                lim[0] = min_element(X);
                lim[1] = max_element(X);
            }

            stringstream ss;
            ss << "c(" << lim[0] << "," << lim[1] << "), ";
            lim_str = ss.str(); 
            return lim_str;
        }

        vector<double> determine_limits (vector< Yseries* > Y) {
            vector<double> lim(2);
            if (Y.size() == 0) {
                cerr << "Cannot determine min and max of empty data vector\n";
                return lim;
            }
            
            for (unsigned int i=0; i<Y.size(); i++) {
                lim[0] = min( lim[0], min_element( Y[i]->data() ) );
                lim[1] = max( lim[1], max_element( Y[i]->data() ) );
            }

            return lim;
        }
};

#endif
