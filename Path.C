#include "Path.h"

Path::Path(){
    
//something?
}

void Path::SetEvent(const Event & ev){
    evptr=&ev;
    nrpts=evptr->NumberofInteractionPts();
    ComputePhotoelectricSigmas();
    ComputeDistances();
}

InteractionPt Path::At(const int & i){
    return evptr->GetInteractionPt(order[i]);
}

//Computing photoelectric cross sections////////////////////////////////////////////////////////////////////

void Path::ComputePhotoelectricSigmas(){
    photosigma.resize(nrpts);
    for (unsigned int i=0; i<nrpts ; i++){
        photosigma[i]=ComputePhotoelectricSigma(evptr->GetInteractionPt(i).GetEnergy());
    }
}

double Path::ComputePhotoelectricSigma(double E){     //presa da OFT!!! controllare!
    /* sigma = 4 alpha^4*sqrt(2)*Z^5 phi_0*(E/mec2)^(-7/2),phi_0=8/3 pi r_0^2 */
    /* sigma abs K+L = 9/8* sigma_Kshell */
    double temp;
    double gamma;
    double hnu_k;
    hnu_k = SQ( Z_GE - 0.03) * MEC2 * SQ( FINE_ALPHA) / 2.0;   //NO NEED TO COMPUTE THIS EVERYTIME!!!
    gamma = CB(E / MEC2) * CB(E / MEC2) * E / MEC2 ;        //AND SLSO THESE QUOTIENTS
    gamma = sqrt(gamma);
    //   temp = 4 * CB(fine_alpha) * fine_alpha * 1.4142 * 6.651e-25 * CB(Z_ge) * SQ(Z_ge);//AND ALSO THIS
    temp = 4 * CB(FINE_ALPHA) * FINE_ALPHA * 1.4142 * 6.651e-23 * CB(Z_GE) * SQ(Z_GE);//AND ALSO THIS
    
    temp = sqrt(E / MEC2) * temp / gamma;       /* en mm2/atom */
    /* not well suited for energies below 20 keV
     removed the 1.125 factor and added sqrt(E/mec2) to fit data */
    if (E < 0.025) {
        temp = 2.2 * pow((hnu_k / E), 2.6666) * 6.3e-18 / SQ(Z_GE);
        if (E < 0.0111)
            temp = temp / 8.5;
    }
    return temp;
}

//Computing distances/////////////////////////////////////////////////////////////////////////////////////
void Path::ComputeDistances(){
    //initialize the distance matrix, elements on the diagonal are the distance from source
    distancematr.resize(nrpts );
    gedistancematr.resize(nrpts);
    
    for (unsigned int i=0; i<nrpts; i++){
        distancematr.at(i).resize(nrpts);
        gedistancematr.at(i).resize(nrpts);
        for (unsigned int j=0; j<i; j++){
            distancematr[i][j]=distancematr[j][i]=evptr->PointsDistance(i, j);
        }
    }
    
    for (unsigned int i=0; i<nrpts; i++){
        distancematr[i][i]=evptr->GetInteractionPt(i).GetPosition().Norm();
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

//Distance in germanium/////////////////////////////////////////////////////////////////////////////////////////////////////////
double Path::DistanceGe(const int &i, const int & j){
    Vec3 A, B;
    double a, b;
    b=distancematr[i][j];
    if (distancematr[i][i]<distancematr[j][j]){
        A=-evptr->GetInteractionPt(i).GetPosition();
        B=evptr->GetInteractionPt(j).GetPosition()+A;
        a=distancematr[i][i];
    }else{
        A=-evptr->GetInteractionPt(j).GetPosition();
        B=evptr->GetInteractionPt(i).GetPosition()+A;
        a=distancematr[j][j];
    }
    double scalar=A*B;
    if(scalar<0){
        return distancematr[i][j];
    }else{
        double rcenter=sqrt(SQ(a)-SQ(scalar)/SQ(b)); //distance from source to line connecting the two points
        if (rcenter>=RINNER){
            return distancematr[i][j];
        }else{
            double rvacuum=sqrt(SQ(RINNER)-SQ(rcenter));
            rvacuum=distancematr[i][j]-2*rvacuum;
            if (rvacuum>0){
                return rvacuum;
            }else{
                return RESOLUTION;
            }
        }
    }
}

//Actual distance in ge keeping in mind the geometry!!
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
double Path::DistanceGe(const int &i){//provvisoria, non thiene conto della geometria
    double dist;
    dist=evptr->GetInteractionPt(i).GetPosition().Norm()-RINNER;
    if (dist>0){
        return dist;
    }else{
        return RESOLUTION;
    }
}


double Path::GetGeDistance(const int & i, const int & j){
    return gedistancematr[order[i]][order[j]];
}

double Path::GetDistance(const int & i, const int & j){
    return distancematr[order[i]][order[j]];
}

double Path::GetInEnergy(const int & i){
    return inenergy[order[i]];
}

double Path::IncidentEnergy(const int & i){
    return inenergy[i];
}

void Path::Clear(){
    order.clear();
    distancematr.clear();
    gedistancematr.clear();
    photosigma.clear();
}

double Path::PhotoSigma(const int & i){
    return photosigma[order[i]];
}

double Path::Distance(const int & i, const int & j){
    return distancematr[order[i]][order[j]];
}

double Path::GeDistance(const int & i, const int & j){
    return gedistancematr[order[i]][order[j]];
}

void Path::OrderPoints(const std::vector <int> & order, const double & etot){
    this->order=order;
    double etmp=0;
    inenergy.resize(nrpts);
    for (unsigned int i=0; i<nrpts; i++){
        inenergy[i]=etot-etmp;
        etmp=evptr->GetInteractionPt(order[i]).GetEnergy();
    }
};



