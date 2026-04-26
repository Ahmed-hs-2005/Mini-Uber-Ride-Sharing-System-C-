#include <bits/stdc++.h>
using namespace std;

/* ===================== STRUCTURES ===================== */

struct Ride {
    int rideNo;
    string riderName;
    double distance;
};

struct Driver {
    int id;
    string name;

    optional<Ride> ongoingRide;
    queue<Ride> assignedRides;

    int totalCompleted = 0;
    double earnings = 0;

    double ratingSum = 0;
    int ratingCount = 0;
};

/* ===================== GLOBAL DATA ===================== */

vector<Driver> drivers;

int rideCounter = 1;

/* ===================== FILE HANDLING ===================== */

void saveCompletedRide(int rideNo, const string& driverName, double fare, double rating) {
    bool fileExists = false;

    ifstream checkFile("completed_rides.csv");
    if (checkFile.good())
        fileExists = true;
    checkFile.close();

    ofstream file("completed_rides.csv", ios::app);

    if (!fileExists) {
        file << "RideNo,DriverName,Fare,Rating\n";
    }

    file << rideNo << "," << driverName << "," << fare << "," << rating << "\n";
    file.close();
}

/* ===================== DRIVER SELECTION ===================== */

Driver* getAvailableDriver() {
    for (auto &d : drivers) {
        int active =
            (d.ongoingRide.has_value() ? 1 : 0) +
            d.assignedRides.size();

        if (active < 3)
            return &d;
    }
    return nullptr;
}

/* ===================== ANALYTICS ===================== */

void showAnalytics() {
    cout << "\n========== DRIVER ANALYTICS ==========\n";

    for (auto &d : drivers) {
        cout << "\nDriver: " << d.name << "\n";

        double avgRating =
            (d.ratingCount == 0) ? 0 : d.ratingSum / d.ratingCount;

        cout << "Average Rating: " << fixed << setprecision(2)
             << avgRating << "/5\n";

        if (d.ongoingRide.has_value()) {
            cout << "Status: ONGOING\n";
            cout << " Ongoing Ride: "
                 << d.ongoingRide->riderName
                 << " (" << d.ongoingRide->distance << " km)\n";
        } else {
            cout << "Status: FREE\n";
        }

        cout << " Assigned Rides:\n";
        if (d.assignedRides.empty()) {
            cout << "  None\n";
        } else {
            queue<Ride> temp = d.assignedRides;
            while (!temp.empty()) {
                auto r = temp.front(); temp.pop();
                cout << "  " << r.riderName
                     << " (" << r.distance << " km)\n";
            }
        }

        cout << "Total Earnings: " << d.earnings << "\n";
        cout << "------------------------------------\n";
    }
}

/* ===================== BOOK RIDE ===================== */

void bookRide() {
    string name;
    double distance;

    cout << "Enter Rider Name: ";
    cin >> name;
    cout << "Enter Distance (km): ";
    cin >> distance;

    Driver* d = getAvailableDriver();
    if (!d) {
        cout << "No driver available.\n";
        return;
    }

    Ride r{rideCounter++, name, distance};

    if (!d->ongoingRide.has_value())
        d->ongoingRide = r;
    else
        d->assignedRides.push(r);

    cout << "Ride booked successfully.\n";
    cout << "Assigned Driver: " << d->name << "\n";
}

/* ===================== CANCEL ASSIGNED RIDE ===================== */

void cancelAssignedRide() {
    showAnalytics();

    string name;
    double distance;

    cout << "Enter Rider Name to cancel: ";
    cin >> name;
    cout << "Enter Distance: ";
    cin >> distance;

    for (auto &d : drivers) {
        queue<Ride> temp;
        bool cancelled = false;

        while (!d.assignedRides.empty()) {
            Ride r = d.assignedRides.front();
            d.assignedRides.pop();

            if (!cancelled &&
                r.riderName == name &&
                r.distance == distance) {
                cancelled = true;
            } else {
                temp.push(r);
            }
        }

        d.assignedRides = temp;

        if (cancelled) {
            cout << "Assigned ride cancelled successfully.\n";
            return;
        }
    }

    cout << "Ride not found or already ongoing.\n";
}

/* ===================== COMPLETE RIDE ===================== */

void completeRide() {
    showAnalytics();

    string name;
    double distance;
    double rating;

    cout << "Enter Rider Name to complete: ";
    cin >> name;
    cout << "Enter Distance: ";
    cin >> distance;
    cout << "Give Rating (1-5): ";
    cin >> rating;

    if (rating < 1 || rating > 5) {
        cout << "Invalid rating.\n";
        return;
    }

    for (auto &d : drivers) {
        if (d.ongoingRide.has_value() &&
            d.ongoingRide->riderName == name &&
            d.ongoingRide->distance == distance) {

            Ride finished = d.ongoingRide.value();
            d.ongoingRide.reset();

            double fare = finished.distance * 150;

            d.earnings += fare;
            d.totalCompleted++;

            d.ratingSum += rating;
            d.ratingCount++;

            saveCompletedRide(
                finished.rideNo,
                d.name,
                fare,
                rating
            );

            if (!d.assignedRides.empty()) {
                d.ongoingRide = d.assignedRides.front();
                d.assignedRides.pop();
            }

            cout << "Ride completed.\n";
            cout << "Fare: " << fare << "\n";
            return;
        }
    }

    cout << "Ongoing ride not found.\n";
}

/* ===================== MENU ===================== */

void menu() {
    cout << "\n========= MINI-UBER =========\n";
    cout << "1. Book Ride\n";
    cout << "2. Cancel Assigned Ride\n";
    cout << "3. Complete Ongoing Ride\n";
    cout << "4. Show Analytics\n";
    cout << "5. Exit\n";
    cout << "Enter choice: ";
}

/* ===================== MAIN ===================== */

int main() {
    drivers = {
        {1,"Ali"},
        {2,"Ahmed"},
        {3,"Usman"},
        {4,"Hassan"},
        {5,"Bilal"}
    };

    int choice;
    do {
        menu();
        cin >> choice;

        if (choice == 1) bookRide();
        else if (choice == 2) cancelAssignedRide();
        else if (choice == 3) completeRide();
        else if (choice == 4) showAnalytics();

    } while (choice != 5);

    cout << "System exited successfully.\n";
    return 0;
}
