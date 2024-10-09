template <typename T>
T* GetObjectFromFile(TFile* file, const std::string& objectName) {
    if (!file || file->IsZombie()) {
        std::cerr << "Error: File is not open or corrupted." << std::endl;
        return nullptr;
    }

    TObject* obj = file->Get(objectName.c_str());
    if (!obj) {
        std::cerr << "Error: Object " << objectName << " not found." << std::endl;
        return nullptr;
    }

    // Attempt to cast to the requested type
    T* typedObj = dynamic_cast<T*>(obj);
    if (!typedObj) {
        std::cerr << "Error: Object " << objectName << " is not of type " << typeid(T).name() << std::endl;
        return nullptr;
    }

    return typedObj;
}

template <typename T>
T diff(const T& a, const T& b) {
    return (a > b) ? (a - b) : (b - a);
}

void Pal_blue()
{
    static Int_t colors[1000];
    static Bool_t initialized = kFALSE;
    Double_t Red[5] = {250. / 255., 178. / 255., 141. / 255., 104. / 255., 67. / 255.};
    Double_t Green[5] = {250. / 255., 201. / 255., 174. / 255., 146. / 255., 120. / 255.};
    Double_t Blue[5] = {250. / 255., 230. / 255., 216. / 255., 202. / 255., 188. / 255.};
    Double_t Length[5] = {0.00, 0.25, 0.50, 0.75, 1.00};
    if (!initialized)
    {
        Int_t FI = TColor::CreateGradientColorTable(5, Length, Red, Green, Blue, 1000);
        for (int i = 0; i < 1000; i++)
            colors[i] = FI + i;
        initialized = kTRUE;
        return;
    }
    gStyle->SetPalette(1000, colors);
}