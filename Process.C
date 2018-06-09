#include "Process.h"

//Initialization procedures///////////////////////////////////////////////////////////////////////////
Process::Process(){
    //Compute front faces here!!
    
}

void Process::LoadEvent(const Event & ev){
    this->ev=ev;
    MergePoints();
    nrpts=this->ev.NumberofInteractionPts();
    
    //  SmearPoints();
    ComputePhotoelectricSigmas();
    if (nrpts!=1){
        ComputeDistances();
    }
}

//Add original event to confront//////////////////////////////////////////////////////////////////////

void Process::AddOriginal(const Event & ev_orig){
    this->ev_orig=ev_orig;
}

//Closing procedures/////////////////////////////////////////////////////////////////////////////////

void Process::ClearAll(){
    ev.Clear();
    //    interactionorder.clear();
    distancematr.clear();
    gedistancematr.clear();
    photosigma.clear();
    totalfactors.clear();
}

//Merging close points/////////////////////////////////////////////////////////////////////////////////

void Process::MergePoints(){
    std::vector<int> number_events;
    for (unsigned int i=0; i<ev.NumberofInteractionPts(); i++){//do not use nrpts
        number_events.push_back(i);
        for (unsigned int j=i+1; j<ev.NumberofInteractionPts();j++){//do not use nrpts
            if (ev.PointsDistance(i, j)<4){
                number_events.push_back(j);
            }
        }
        if(number_events.size()>1){
            ev.MergePoints(number_events);
        }
        number_events.clear();
    }
}

//Computing photoelectric cross sections//////////////////////////////////////////////////////////////

void Process::ComputePhotoelectricSigmas(){
    for (unsigned int i=0; i<nrpts ; i++){
        photosigma.push_back(ComputePhotoelectricSigma(ev.GetInteractionPt(i).GetEnergy()));
    }
}

double Process::ComputePhotoelectricSigma(double E){     //presa da OFT!!! controllare!
    /* sigma = 4 alpha^4*sqrt(2)*Z^5 phi_0*(E/mec2)^(-7/2),phi_0=8/3 pi r_0^2 */
    /* sigma abs K+L = 9/8* sigma_Kshell */
    double temp;
    double gamma;
    double hnu_k;
    hnu_k = SQ(Z_ge - 0.03) * mec2 * SQ(fine_alpha) / 2;   //NO NEED TO COMPUTE THIS EVERYTIME!!!
    gamma = CB(E / mec2) * CB(E / mec2) * E / mec2;        //AND SLSO THESE QUOTIENTS
    gamma = sqrt(gamma);
    //   temp = 4 * CB(fine_alpha) * fine_alpha * 1.4142 * 6.651e-25 * CB(Z_ge) * SQ(Z_ge);//AND ALSO THIS
    temp = 4 * CB(fine_alpha) * fine_alpha * 1.4142 * 6.651e-23 * CB(Z_ge) * SQ(Z_ge);//AND ALSO THIS
    
    temp = sqrt(E / mec2) * temp / gamma;       /* en mm2/atom */
    /* not well suited for energies below 20 keV
     removed the 1.125 factor and added sqrt(E/mec2) to fit data */
    if (E < 0.025) {
        temp = 2.2 * pow((hnu_k / E), 2.6666) * 6.3e-18 / SQ(Z_ge);
        if (E < 0.0111)
            temp = temp / 8.5;
    }
    return temp;
}

//Computing distances/////////////////////////////////////////////////////////////////////////////////////
void Process::ComputeDistances(){
    //initialize the distance matrix, elements on the diagonal are the distance from source
    distancematr.resize(nrpts);
    gedistancematr.resize(nrpts);
    
    for (unsigned int i=0; i<nrpts; i++){
        distancematr.at(i).resize(nrpts);
        gedistancematr.at(i).resize(nrpts);
        for (unsigned int j=0; j<i; j++){
            distancematr[i][j]=distancematr[j][i]=ev.PointsDistance(i, j);
        }
    }
    
    for (unsigned int i=0; i<nrpts; i++){
        distancematr[i][i]=ev.GetInteractionPt(i).GetPosition().Norm();
    }
    
    //call to compute the distance in germanium
    for (unsigned int i=0; i<nrpts; i++){
        for (unsigned int j=0; j<i; j++){
            gedistancematr[i][j]=gedistancematr[j][i]=DistanceGe(i, j);
        }
    }
    for (unsigned int i=0; i<nrpts; i++){
        gedistancematr[i][i]=DistanceGe(i);
    }
}

//Scattering angles//////////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeScatteringCosAngle(const int & j, const int & k){//maybe matrix?
    Vec3 A=ev.GetInteractionPt(j).GetPosition();
    return A.CosAngleBetween(ev.GetInteractionPt(k).GetPosition()-A);
}

double Process::ComputeScatteringCosAngle(const int & i, const int & j, const int & k){//maype cubic matrix?
    Vec3 A=ev.GetInteractionPt(j).GetPosition();
    return (A-ev.GetInteractionPt(i).GetPosition()).CosAngleBetween((ev.GetInteractionPt(k).GetPosition()-A));
}


//Compton kinematic factor////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeComptonFactor(const int & i, const int & j, const int & k, const double & E1,  const double & E2){
    double cosang=ComputeScatteringCosAngle(i, j, k);
    double Egeo=E1/(1.0+E1/mec2*(1-cosang));//sistemare
   // double err2=SQ(sigma_E)*SQ(Egeo/E1*(1+Egeo*(1-cosang)/mec2))+SQ(mec2*SQ(E2)/SQ(mec2+E2*(1-cosang)))*SQ(2*resolution)/(SQ(distancematr[j][k])+SQ(resolution)); //rough approximation, not even sure acout d(cos)/dr
    double err2=16;
//    std::cout<<"COMPTON "<<i <<" "<< j << " " << k <<" energia  out "<< Egeo<<" Energia in "<<E1<<std::endl; 
    return exp(-SQ(E2-Egeo)/E2);
}

double Process::ComputeComptonFactor(const int & i, const int & j, const double & E1,  const double & E2){
    double cosang=ComputeScatteringCosAngle(i, j);
    double Egeo=E1/(1.0+E1/mec2*(1.0-cosang));//sistemare
//    double err2=SQ(sigma_E)*SQ(Egeo/E1*(1+Egeo*(1-cosang)/mec2))+SQ(mec2*SQ(E2)/SQ(mec2+E2*(1-cosang)))*SQ(2*resolution)/(SQ(distancematr[i][j])+SQ(resolution));
    double err2=16;
    double answer=exp(-SQ(E2-Egeo)/E2);
//    std::cout<<"COMPTON "<< i << " " << j << " energia  out "<< Egeo<<" Energia in "<<E1<<std::endl; 
    return answer; //tialiere questo step
}


//Compton cross section////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeNishinaSigma(const int & i, const int & j, const int & k, const double & E1,  const double & E2){
    return 0.5*SQ(r0)*SQ(E2/E1)*(E2/E1+E1/E2-1+SQ(ComputeScatteringCosAngle(i, j, k))); //sistemare
}

double Process::ComputeNishinaSigma(const int & i, const int & j, const double & E1,  const double & E2){
    return 0.5*SQ(r0)*SQ(E2/E1)*(E2/E1+E1/E2-1+SQ(ComputeScatteringCosAngle(i, j))); //sistemare
}

//Nishina cross section//////////////////////////////////////////////////////////////////////////////////
/*double Process::ComputeNishinaSigmaTotal(const double & E){//controllare!!!!!!
 double temp;
 double temp0;
 double temp1;
 double temp2;
 double temp3;
 double gamma;
 gamma = E / mec2;
 
 temp0 = 1e4 * 2 * 3.1415 * SQ(r0) * Z_ge;  // in cm2/atom
 temp1 = 1 + gamma;
 temp2 = 1 + (2 * gamma);
 temp3 = 1 + (3 * gamma);
 temp = (temp1 / SQ(gamma)) * (((2 * temp1) / temp2) - (log(temp2) / gamma));
 temp = temp + (log(temp2) / (2 * gamma) - (temp3 / SQ(temp2)));
 temp = temp * temp0;
 return temp*100.0;
 }*/


double Process::ComputeNishinaSigmaTotal(const double & E){
    //http://lappweb.in2p3.fr/~maire/tutorials/compton.pdf
    double e=E/mec2;
    return 2*pi*SQ(r0)*Z_ge*(((SQ(e)-2*e-2)/(2*CB(e)))*log(2*e+1)+(CB(e)+9*SQ(e)+8*e+2)/(4*SQ(e)*SQ(e)+4*CB(e)+SQ(e)));
}


//Germanium distances///////////////////////////////////////////////////////////////////////////////////
double Process::DistanceGe(const int &i, const int & j){
    Vec3 A, B;
    double a, b;
    b=distancematr[i][j];
    if (distancematr[i][i]<distancematr[j][j]){
        A=-ev.GetInteractionPt(i).GetPosition();
        B=ev.GetInteractionPt(j).GetPosition()+A;
        a=distancematr[i][i];
    }else{
        A=-ev.GetInteractionPt(j).GetPosition();
        B=ev.GetInteractionPt(i).GetPosition()+A;
        a=distancematr[j][j];
    }
    double scalar=A*B;
    if(scalar<0){
        return distancematr[i][j];
    }else{
        double rcenter=sqrt(SQ(a)-SQ(scalar)/SQ(b)); //distance from source to line connecting the two points
        if (rcenter>=rinner){
            return distancematr[i][j];
        }else{
            double rvacuum=sqrt(SQ(rinner)-SQ(rcenter));
            rvacuum=distancematr[i][j]-2*rvacuum;
            if (rvacuum>0){
                return rvacuum;
            }else{
                return resolution;
            }
        }
    }
}

//Actual distance in ge keeping in mind the geometry//////////////////////////////////////////////////////
/*
 double Process::DistanceGe(const int &i){
 double dist;
 dist=ev.GetInteractionPt(i).GetPosition().Norm()*(1-SQ(rinner)/(frontfaces[ev.GetInteractionPt(i).GetDetector()]*ev.GetInteractionPt(i).GetPosition()));
 if (dist>0){
 return dist;
 }else{
 return resolution;
 }
 }*/
double Process::DistanceGe(const int &i){//temporary, does not take into account the geometry
    double dist;
    dist=ev.GetInteractionPt(i).GetPosition().Norm()-rinner;
    if (dist>0){
        return dist;
    }else{
        return resolution;
    }
}


//Total merit factor//////////////////////////////////////////////////////////////////////////////////////
double Process::ComputeTotalFactor(const std::vector <int> & interactionorder, std::vector <meritfactor> & meritfactors, const double & etot){
	long double ptmp=-1;

	size_t size=interactionorder.size();
	double E1=etot;
	double E2=E1-ev.GetInteractionPt(interactionorder[0]).GetEnergy();
	double sigma=-1;
	unsigned int i=1;

#undef SETNR

	if (1){
		// std::cout << "sz int "<<interactionorder.size()<< " sz mf "<<meritfactors.size()<<"\n";
		std::cout<<"Ordine interazione"<<std::endl;
		for (int ss=0;ss<interactionorder.size(); ss++){
			std::cout<<"   "<<interactionorder[ss]<<" ";
		}
		std::cout<<std::endl;
	}

	if (size==1){//maybe not needed, if loop closes right IT IS A MESS::::::CHECK EVERTHING IN THIS METHOD
		sigma=photosigma[interactionorder[0]];
		meritfactors[0].nr=-1;
		return NrhA*sigma*exp(-NrhA*sigma*gedistancematr[interactionorder[0]][interactionorder[0]]);
		//         return 1;
	}else{
		while (meritfactors[i].nr==interactionorder[i]){
			if(i!=1) {
				E1=E2;
				E2=E1-ev.GetInteractionPt(interactionorder[i-1]).GetEnergy();
			}
			std::cout<<"ENERGIE LOOP!!!  i="<<i<<" "<< E1<<"  " <<E2<<std::endl<<std::endl;
			i++;
		}
		for(; i<size; i++){
			if (i==1){
				sigma=ComputeNishinaSigmaTotal(E1);
				ptmp=/*NrhA*/exp(-sigma*NrhA*gedistancematr[interactionorder[i-1]][interactionorder[i-1]]);
				ptmp*=ComputeNishinaSigma(interactionorder[i-1], interactionorder[i], E1, E2);
				ptmp=ComputeComptonFactor(interactionorder[i-1], interactionorder[i], E1, E2);
				std::cout<<"ENERGIE!!!  i="<<i<<" "<< E1<<"  " <<E2<<std::endl<<std::endl;
				meritfactors[i-1].factor=ptmp;
#ifdef SETNR
				meritfactors[i-1].nr=interactionorder[i-1];
#endif
			}else{
				E1=E2;
				E2=E1-ev.GetInteractionPt(interactionorder[i-1]).GetEnergy();
				ptmp=meritfactors[i-2].factor;
				sigma=ComputeNishinaSigmaTotal(E1);

				std::cout<<"ENERGIE!!!  i="<<i<<" "<< E1<<"  " <<E2<<std::endl<<std::endl;
				ptmp*=ComputeComptonFactor(interactionorder[i-2], interactionorder[i-1], interactionorder[i], E1, E2);
				ptmp*=ComputeNishinaSigma(interactionorder[i-2], interactionorder[i-1], interactionorder[i], E1, E2);
				ptmp*=/*NrhA*/exp(-NrhA*sigma*gedistancematr[interactionorder[i-2]][interactionorder[i-1]]);
				meritfactors[i-1].factor=ptmp;
#ifdef SETNR
				meritfactors[i-1].nr=interactionorder[i-1];
#endif
			}
		}
		//        sigma=photosigma[interactionorder[i-1]];
		meritfactors[i-1].factor=meritfactors[i-2].factor*sigma*NrhA*exp(-NrhA*sigma*gedistancematr[interactionorder[i-2]][interactionorder[i-1]]);
#ifdef SETNR
		meritfactors[i-1].nr=interactionorder[i-1];
#endif
		return meritfactors[i-1].factor;
	}
}

//Computes all permutations of the fragmented event////////////////////////////////////////////////////

finalevent Process::ComputeDoubleProbability(){
    std::vector <int> interactionorder;
    double p1=0, p2=0;
    double ptotal=0, ptmp=0;
    double etot1=0, etot2=0;
    finalevent final;
    std::vector<meritfactor> meritfactors1;
    std::vector<meritfactor> meritfactors2;
    interactionorder.resize(nrpts);
    for (unsigned int i=0; i<nrpts; i++){
        interactionorder[i]=i;
    }
    
    for(unsigned int it=1; it<interactionorder.size(); it++){
        std::vector<int> part1(&interactionorder.front(), &interactionorder[it]);
        std::vector<int> part2(&interactionorder[it], &interactionorder[interactionorder.size()]);
        meritfactors1.resize(part1.size());
        meritfactors2.resize(part2.size());
        etot1=etot2=0;
        for(unsigned int i=0; i<it;i++){
            meritfactors1[i].nr=-1;
            etot1+=ev.GetInteractionPt(i).GetEnergy();
        }
        for(unsigned int i=0; i<meritfactors2.size();i++){//e qui
            meritfactors2[i].nr=-1;
            etot2+=ev.GetInteractionPt(i+it).GetEnergy();//e qui
        }
        
        do{
            p1=ComputeTotalFactor(part1, meritfactors1, etot1);
            do{
                p2=ComputeTotalFactor(part2, meritfactors2, etot2);
                ptmp=p1*p2;
                if (ptmp>ptotal || ptotal==0){
                    ptotal=ptmp;
                    final.factor=ptotal;
                    final.order1=part1;
                    final.order2=part2;
                }
            }while (std::next_permutation(part2.begin(), part2.end()));
        }while (std::next_permutation(part1.begin(), part1.end()));
    }
//    final.factor=pow(final.factor, 1.0/(ev.NumberofInteractionPts()-1));
    return final;
}


//Computes all permutations of the entire event///////////////////////////////////////////////////////////

finalevent Process::ComputeSingleProbability(){
    std::vector <int> interactionorder;
    double p=0;
    double pfinal=0;
    double etot=ev.GetTotalEnergy();
    finalevent final;
    final.order2.push_back(0);
    std::vector<meritfactor> meritfactors;
    
    meritfactors.resize(nrpts);
    interactionorder.resize(nrpts);
    
    for (unsigned int i=0; i<nrpts; i++){
        meritfactors[i].nr=-2;
        interactionorder[i]=i;
    }
    do{
        p=ComputeTotalFactor(interactionorder, meritfactors, etot);
        if (p>pfinal){
            pfinal=p;
            
            final.factor=pfinal;
            final.order1=interactionorder;
        }
    }while (std::next_permutation(interactionorder.begin(), interactionorder.end()));
//    final.factor=pow(final.factor, 1.0/(ev.NumberofInteractionPts()-1));//added to try
    return final;
}


//Evaluation of the event/////////////////////////////////////////////////////////////////////////////////////
void Process::EvaluateEvent(std::ostream & out, int & number_of_single, int & number_of_double){
	double good1, good2;
	std::cout <<"Computing single probability" <<std::endl;
	finalevent psingle=ComputeSingleProbability();
	std::cout<<psingle.factor<<"\n";
	std::cout <<"Computing double probability" <<std::endl;
	if (nrpts>1){
		finalevent pdouble=ComputeDoubleProbability();
		std::cout<<pdouble.factor<<"\n";
		good1=(ev.GetInteractionPt(pdouble.order1[0]).GetDirection()-ev_orig.GetInteractionPt(0).GetDirection()).Norm();
		good2=(ev.GetInteractionPt(pdouble.order2[0]).GetDirection()-ev_orig.GetInteractionPt(1).GetDirection()).Norm();
		//		std::cout<<"Percentage difference from original directions: "<<good1<<"        "<<good2 <<"\n";
		if (good1>0.05||good2>0.10){
			std::cout<<"This event has more than 0.10 difference!!!!\n";
		}
		//		double factor= (pdouble.factor-psingle.factor)/psingle.factor;
		if (pdouble.factor>psingle.factor ||psingle.factor>1){
			if ( pdouble.factor<=1 && pdouble.factor>0.000001&&ev.NumberofInteractionPts()>3){
				number_of_double++;
				std::cout<<"I chose double, number of int points= "<<ev.NumberofInteractionPts()<<"\n\n\n";
				
			}
		}
		if (psingle.factor>pdouble.factor ||pdouble.factor>1){
			if ( psingle.factor<=1 && psingle.factor>0.000001){

				std::cout<<"I chose single, number of int points= "<<ev.NumberofInteractionPts()<<"\n\n\n";
				number_of_single++;
			}
		}
	}
	std::cout<<"NUMBER OF SINGLE : "<<number_of_single<<"\n";
	std::cout<<"NUMBER OF DOUBLE : "<<number_of_double<<" \n\n\n\n\n\n\n\n\n";
}

void Process::Print(std::ostream & out, const int & a, const int & b){
    out <<"$"<<std::endl;
    out <<ev.GetInteractionPt(a)<<std::endl;
    out <<ev.GetInteractionPt(b)<<std::endl;
}
