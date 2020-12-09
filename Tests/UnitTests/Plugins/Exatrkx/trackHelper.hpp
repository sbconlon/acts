std::vector<std::vector<const SpacePoint*>> truth_to_tracks(std::vector<const SpacePoint*> hits) {

    std::vector<std::vector<const SpacePoint *>> tracks;
    for (auto hitIter=hits.begin(); hitIter!=hits.end(); ++hitIter) {
        auto trkIter=tracks.begin();
        for ( ; trkIter!=tracks.end(); ++trkIter) {
            if (((*trkIter)[0])->ids->pid() == (*hitIter)->ids->pid()) {
                auto it=(*trkIter).begin();
                for ( ; it!=(*trkIter).end(); ++it) {
                    if ((*it)->r() > (*hitIter)->r()) {
                        (*trkIter).insert(it, (*hitIter));
                        break;
                    }
                }
                if (it==(*trkIter).end()) {
                    (*trkIter).push_back((*hitIter));
                }
                break;
            }
        }
        if (trkIter==tracks.end()) {
            std::vector<const SpacePoint*> new_track;
            new_track.push_back(*hitIter);
            tracks.push_back(new_track);
        }
    }
    return tracks;
}


void print_track(std::vector<std::vector<const SpacePoint*>> tracks, int idx) {
    std::vector <const SpacePoint*> trk = tracks[idx];
    std::cout << "Example Size: " << trk.size() << std::endl;
    std::cout << " HID     " << " PID            " << "   R   "<< std::endl;
    for (auto ex=trk.begin(); ex!=trk.end(); ++ex) {
        std::cout << (*ex)->ids->hid() << "  " << (*ex)->ids->pid() << "    " << "        " <<  (*ex)->r() << std::endl;
    }
}


std::vector<Acts::Seed<SpacePoint>> tracks_to_dplet_seeds(std::vector<std::vector<const SpacePoint *>> tracks) {

    std::vector<Acts::Seed<SpacePoint>> seeds;
    const SpacePoint &mSp = *(new SpacePoint{0, 0, 0});                                           // Dummy spacepoint to fill middle point in the seed
    for (auto trkIter=tracks.begin(); trkIter!=tracks.end(); ++trkIter) {
        for (auto hit1=(*trkIter).begin(); hit1!=(*trkIter).end(); ++hit1) {
            auto hit2 = hit1 + 1;
            while(hit2!=(*trkIter).end() && (*hit1)->vols == (*hit2)->vols) { ++hit2; }             // Screen for duplicate hits
            Volumes* next_vols = (*hit2)->vols;
	    while(hit2!=(*trkIter).end() && (*hit2)->vols == next_vols) {                           // Use all hits in the next layer as a seed
                const SpacePoint &bSp = **hit1;
                const SpacePoint &tSp = **hit2;
                seeds.push_back(Acts::Seed<SpacePoint>(bSp, mSp, tSp, 0));
                ++hit2;
            }
        }
    }
    return seeds;
}

std::vector<Acts::Seed<SpacePoint>> tracks_to_tplet_seeds(std::vector<std::vector<const SpacePoint *>> tracks) {

    std::vector<Acts::Seed<SpacePoint>> seeds;
    for (auto trkIter=tracks.begin(); trkIter!=tracks.end(); ++trkIter) {
        for (auto hit1=(*trkIter).begin(); hit1!=(*trkIter).end(); ++hit1) {                      // bottom hit loop
            auto hit2 = hit1 + 1;
            while (hit2!=(*trkIter).end() && (*hit1)->vols == (*hit2)->vols) { ++hit2; }          // screen for duplicate bottom hits
            Volumes* second_vols = (*hit2)->vols;
            for (; hit2!=(*trkIter).end() && (*hit2)->vols == second_vols; ++hit2) {              // middle hit loop (use all hits in 2nd layer)
                auto hit3 = hit2 + 1;
                while (hit3!=(*trkIter).end() && (*hit2)->vols >= (*hit3)->vols) { ++hit3; }       // screen for duplicate middle hits
                Volumes* third_vols = (*hit3)->vols;
                for (; hit3!=(*trkIter).end() && (*hit3)->vols == third_vols; ++hit3) {            // top hit loop (use all hits in 3rd layer)
                    const SpacePoint &bSp = **hit1;
                    const SpacePoint &mSp = **hit2;
                    const SpacePoint &tSp = **hit3;
                    seeds.push_back(Acts::Seed<SpacePoint>(bSp, mSp, tSp, 0));
                }
            }
        }
    }
    return seeds;
}
