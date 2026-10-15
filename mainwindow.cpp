#include "mainwindow.h"
#include <stdexcept> 
#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTime>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <exception>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QDebug>
#include <QJsonParseError>

namespace {
const QString PURPLE = "#6C4DFF";
const QString DARK = "#211D3A";
const QString MUTED = "#77738F";
const QString BG = "#F5F4FB";
const QString BORDER = "#E3E0F0";

QLabel *makeLabel(const QString &text, int size = 10, bool bold = false,
                  const QString &color = DARK)
{
    auto *l = new QLabel(text);
    l->setStyleSheet(QString("color:%1;font-size:%2px;%3")
        .arg(color).arg(size).arg(bold ? "font-weight:700;" : ""));
    return l;
}

QFrame *makeCard()
{
    auto *f = new QFrame;
    f->setObjectName("card");
    f->setStyleSheet("#card { background:#FFFFFF; border:1px solid " + BORDER + "; border-radius:16px; }");
    return f;
}

QPushButton *makeButton(const QString &text, bool primary = true)
{
    auto *b = new QPushButton(text);
    b->setCursor(Qt::PointingHandCursor);
    b->setMinimumHeight(42);
    b->setStyleSheet(primary
        ? "QPushButton{background:#6C4DFF;color:white;border:0;border-radius:10px;padding:10px 16px;font-weight:700;} QPushButton:hover{background:#5238DC;} QPushButton:pressed{background:#3E2AB2;}"
        : "QPushButton{background:#F1EEFF;color:#4933C7;border:1px solid #DCD5FF;border-radius:10px;padding:10px 16px;font-weight:700;} QPushButton:hover{background:#E6E0FF;}");
    return b;
}

QLineEdit *makeLineEdit(const QString &placeholder)
{
    auto *e = new QLineEdit;
    e->setPlaceholderText(placeholder);
    e->setMinimumHeight(42);
    e->setStyleSheet("QLineEdit{background:#FFFFFF;color:#211D3A;border:1px solid #D8D4E8;border-radius:9px;padding:9px;font-size:13px;} QLineEdit:focus{border:2px solid #6C4DFF;} QLineEdit::placeholder{color:#9995AD;}");
    return e;
}

QLabel *makeHeading(const QString &text)
{
    return makeLabel(text, 21, true, DARK);
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("RideShare | Smart Mobility");
    resize(1180, 780);
    setMinimumSize(980, 680);
    setStyleSheet("QMainWindow{background:" + BG + ";} QWidget{font-family:'Segoe UI',Arial;} QLabel{background:transparent;}");

    auto *root = new QWidget;
    auto *rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *sidebar = new QWidget;
    sidebar->setFixedWidth(205);
    sidebar->setStyleSheet("background:#211D3A;");
    auto *side = new QVBoxLayout(sidebar);
    side->setContentsMargins(18, 24, 18, 20);
    side->setSpacing(10);
    auto *brand = makeLabel("◈  RideShare", 21, true, "#FFFFFF");
    brand->setContentsMargins(5, 8, 0, 26);
    side->addWidget(brand);
    side->addWidget(makeLabel("SMART MOBILITY", 9, true, "#AFA8D5"));

    const QStringList navNames = {"⌂   Dashboard", "＋   Book a Ride", "▤   My Rides", "◷   Journey Monitor"};
    for (int i = 0; i < navNames.size(); ++i) {
        auto *nav = new QPushButton(navNames[i]);
        nav->setCursor(Qt::PointingHandCursor);
        nav->setMinimumHeight(44);
        nav->setStyleSheet("QPushButton{color:#E7E3FF;text-align:left;padding-left:12px;background:transparent;border:0;border-radius:9px;font-size:12px;} QPushButton:hover{background:#373052;color:white;}");
        side->addWidget(nav);
        connect(nav, &QPushButton::clicked, this, [this, i]() { navigateTo(i); });
    }
    side->addStretch();
    side->addWidget(makeLabel("SAFE RIDES.\nBETTER JOURNEYS.", 10, true, "#BEB8DC"));
    side->addWidget(makeLabel("SMARTER EVERYDAY TRAVEL", 8, false, "#8E87B4"));

    auto *content = new QWidget;
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(28, 22, 28, 22);
    contentLayout->setSpacing(18);
    auto *top = new QHBoxLayout;
    auto *topText = new QVBoxLayout;
    topText->addWidget(makeLabel("RIDE BOOKING PLATFORM", 9, true, PURPLE));
    topText->addWidget(makeHeading("Your next ride, made simple."));
    topText->addWidget(makeLabel("Plan a trip, estimate your fare and manage your rides.", 11, false, MUTED));
    top->addLayout(topText);
    top->addStretch();
    contentLayout->addLayout(top);

    pages = new QStackedWidget;
    pages->addWidget(makeDashboardPage());
    pages->addWidget(makeBookingPage());
    pages->addWidget(makeHistoryPage());
    pages->addWidget(makeJourneyPage());
    contentLayout->addWidget(pages, 1);
    rootLayout->addWidget(sidebar);
    rootLayout->addWidget(content, 1);
    setCentralWidget(root);

    journeyTimer = new QTimer(this);
    journeyTimer->setInterval(1000);
    connect(journeyTimer, &QTimer::timeout, this, &MainWindow::updateJourneyTimer);
    loadRides();

updateFare();
updateDashboard();
refreshHistory();
}

QWidget *MainWindow::makeDashboardPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    auto *hero = new QFrame;
    hero->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #6C4DFF,stop:1 #4030B7);border-radius:18px;");
    auto *heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(24, 22, 24, 22);
    auto *heroText = new QVBoxLayout;
    heroText->addWidget(makeLabel("YOUR JOURNEY STARTS HERE", 9, true, "#E3DDFF"));
    heroText->addWidget(makeLabel("Where would you like\nto go today?", 23, true, "#FFFFFF"));
    heroText->addWidget(makeLabel("Comfortable rides. Clear pricing. Easy trip management.", 10, false, "#EEE9FF"));
    auto *heroBook = makeButton("Plan a Ride  →");
    heroLayout->addLayout(heroText, 1);
    heroLayout->addWidget(heroBook, 0, Qt::AlignBottom);
    connect(heroBook, &QPushButton::clicked, this, [this]() { navigateTo(1); });
    layout->addWidget(hero);

    auto *stats = new QHBoxLayout;
    auto addStat = [stats](const QString &title, const QString &value) {
        auto *f = makeCard();
        auto *l = new QVBoxLayout(f);
        l->setContentsMargins(17, 14, 17, 14);
        l->addWidget(makeLabel(title, 10, true, MUTED));
        auto *v = makeLabel(value, 22, true, DARK);
        v->setObjectName(title == "TOTAL BOOKINGS" ? "totalBookingsValue" : (title == "CURRENT TRIP" ? "currentTripValue" : "latestDistanceValue"));
        l->addWidget(v);
        stats->addWidget(f);
    };
    addStat("TOTAL BOOKINGS", "0");
    addStat("CURRENT TRIP", "No active ride");
    addStat("LATEST DISTANCE", "—");
    layout->addLayout(stats);

    auto *quick = makeCard();
    auto *quickLayout = new QVBoxLayout(quick);
    quickLayout->setContentsMargins(20, 18, 20, 18);
    quickLayout->addWidget(makeLabel("Quick actions", 15, true));
    auto *actions = new QHBoxLayout;
    auto *book = makeButton("＋  Book a Ride");
    auto *history = makeButton("▤  View Ride History", false);
    auto *journey = makeButton("◷  Journey Monitor", false);
    actions->addWidget(book); actions->addWidget(history); actions->addWidget(journey);
    quickLayout->addLayout(actions);
    connect(book, &QPushButton::clicked, this, [this]() { navigateTo(1); });
    connect(history, &QPushButton::clicked, this, [this]() { navigateTo(2); });
    connect(journey, &QPushButton::clicked, this, [this]() { navigateTo(3); });
    layout->addWidget(quick);

    auto *note = makeCard();
    auto *noteLayout = new QVBoxLayout(note);
    noteLayout->setContentsMargins(20, 16, 20, 16);
    noteLayout->addWidget(makeLabel("Designed for a smooth ride experience", 14, true));
    noteLayout->addWidget(makeLabel("Manage bookings, review past trips and track journey time. Distances are illustrative estimates, not live routing.", 10, false, MUTED));
    layout->addWidget(note);
    layout->addStretch();
    return page;
}

QWidget *MainWindow::makeBookingPage()
{
    auto *page = new QWidget;
    auto *layout = new QHBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);
    auto *form = makeCard();
    auto *f = new QVBoxLayout(form);
    f->setContentsMargins(22, 22, 22, 22);
    f->setSpacing(12);
    f->addWidget(makeHeading("Book your ride"));
    f->addWidget(makeLabel("Passenger details", 10, true, MUTED));
    nameInput = makeLineEdit("Full name");
    phoneInput = makeLineEdit("10-digit mobile number");
    phoneInput->setMaxLength(10);
    f->addWidget(nameInput); f->addWidget(phoneInput);
    f->addSpacing(4);
    f->addWidget(makeLabel("Route details", 10, true, MUTED));
    pickupInput = makeLineEdit("Pickup location (e.g. Viman Nagar, Pune)");
    destinationInput = makeLineEdit("Destination (e.g. FC Road, Pune)");
    f->addWidget(pickupInput); f->addWidget(destinationInput);
    f->addWidget(makeLabel("Vehicle", 10, true, MUTED));
    vehicleInput = new QComboBox;
    vehicleInput->addItems({"Mini", "Sedan", "SUV"});
    vehicleInput->setMinimumHeight(43);
    vehicleInput->setStyleSheet("QComboBox{background:white;color:#211D3A;border:1px solid #D8D4E8;border-radius:9px;padding:8px 10px;min-height:24px;} QComboBox::drop-down{border:0;width:28px;} QComboBox QAbstractItemView{background:white;color:#211D3A;selection-background-color:#6C4DFF;selection-color:white;outline:0;padding:4px;}");
    f->addWidget(vehicleInput);

    auto *estimateCard = new QFrame;
    estimateCard->setObjectName("estimateCard");
    estimateCard->setMinimumHeight(130);
    estimateCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    estimateCard->setStyleSheet("QFrame#estimateCard{background-color:#30245F;border:1px solid #4D3B91;border-radius:14px;}");
    auto *estimateLayout = new QVBoxLayout(estimateCard);
    estimateLayout->setContentsMargins(18, 14, 18, 14);
    estimateLayout->setSpacing(8);
    estimateLayout->addWidget(makeLabel("ESTIMATED FARE", 10, true, "#D8CEFF"));
    fareValue = makeLabel("₹0.00", 26, true, "#FFFFFF");
    estimateLayout->addWidget(fareValue);
    distanceValue = makeLabel("Estimated distance: —", 11, false, "#E5DFFF");
    distanceValue->setWordWrap(true);
    estimateLayout->addWidget(distanceValue);
    f->addWidget(estimateCard);

    auto *book = makeButton("Confirm Booking  →");
    auto *clear = makeButton("Clear Form", false);
    f->addWidget(book); f->addWidget(clear); f->addStretch();

    auto *sideLayout = new QVBoxLayout;
    auto *vehicleCard = makeCard();
    auto *vc = new QVBoxLayout(vehicleCard);
    vc->setContentsMargins(20, 20, 20, 20);
    vc->addWidget(makeHeading("Choose your comfort"));
    vc->addWidget(makeLabel("Choose a ride that fits your journey.", 10, false, MUTED));
    vc->addWidget(makeLabel("Mini  •  Affordable, up to 4 seats", 10, true));
    vc->addWidget(makeLabel("Sedan  •  Extra comfort, up to 4 seats", 10, true));
    vc->addWidget(makeLabel("SUV  •  More room, up to 6 seats", 10, true));
    sideLayout->addWidget(vehicleCard);
    auto *assist = makeCard();
    auto *al = new QVBoxLayout(assist);
    al->setContentsMargins(20, 20, 20, 20);
    al->addWidget(makeLabel("RideShare features", 15, true));
    al->addWidget(makeLabel("✓ Passenger validation\n✓ Ride status management\n✓ Fare estimation\n✓ Journey time tracking", 11, false, MUTED));
    sideLayout->addWidget(assist);
    sideLayout->addStretch();
    auto *sideWidget = new QWidget;
    sideWidget->setLayout(sideLayout);
    layout->addWidget(form, 3);
    layout->addWidget(sideWidget, 2);

    connect(vehicleInput, &QComboBox::currentTextChanged, this, [this]() { updateFare(); });
    connect(pickupInput, &QLineEdit::textChanged, this, [this]() { updateFare(); });
    connect(destinationInput, &QLineEdit::textChanged, this, [this]() { updateFare(); });
    connect(book, &QPushButton::clicked, this, &MainWindow::bookRide);
    connect(clear, &QPushButton::clicked, this, [this]() {
        nameInput->clear(); phoneInput->clear(); pickupInput->clear(); destinationInput->clear();
        vehicleInput->setCurrentIndex(0); updateFare();
    });
    return page;
}

QWidget *MainWindow::makeHistoryPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    auto *panel = makeCard();
    auto *pl = new QVBoxLayout(panel);
    pl->setContentsMargins(20, 20, 20, 20);
    auto *top = new QHBoxLayout;
    top->addWidget(makeHeading("My rides")); top->addStretch();
    auto *refresh = makeButton("Refresh", false);
    auto *details = makeButton("View Selected Ride");
    auto *cancel = makeButton("Cancel Selected", false);
    top->addWidget(refresh); top->addWidget(details); top->addWidget(cancel);
    pl->addLayout(top);
    historyTable = new QTableWidget(0, 6);
    historyTable->setHorizontalHeaderLabels({"Date & Time", "Passenger", "Route", "Vehicle", "Fare", "Status"});
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->verticalHeader()->setVisible(false);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->setAlternatingRowColors(true);
    historyTable->setStyleSheet("QTableWidget{background:white;color:#211D3A;gridline-color:#EEEAF8;border:1px solid #E3E0F0;border-radius:8px;alternate-background-color:#F9F8FE;} QHeaderView::section{background:#F0EDFA;color:#4B4568;padding:9px;border:0;font-weight:700;}");
    pl->addWidget(historyTable);
    layout->addWidget(panel);
    connect(refresh, &QPushButton::clicked, this, &MainWindow::refreshHistory);
    connect(details, &QPushButton::clicked, this, [this]() {
        int row = historyTable->currentRow();
        if (row < 0) { QMessageBox::information(this, "Select a ride", "Select a ride in the table first."); return; }
        showRideDetails(historyTable->item(row, 0)->data(Qt::UserRole).toInt());
    });
    connect(cancel, &QPushButton::clicked, this, &MainWindow::cancelSelectedRide);
    return page;
}

QWidget *MainWindow::makeJourneyPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);
    auto *panel = makeCard();
    auto *pl = new QVBoxLayout(panel);
    pl->setContentsMargins(24, 24, 24, 24);
    pl->addWidget(makeHeading("Journey monitor"));
    pl->addWidget(makeLabel("Track your trip duration and view a summary when you finish.", 10, false, MUTED));
    auto *journeyStatus = makeLabel("No active journey", 16, true, PURPLE);
    journeyStatus->setObjectName("journeyStatusValue");
    pl->addWidget(journeyStatus);
    timerValue = makeLabel("00:00:00", 36, true, DARK);
    pl->addWidget(timerValue);
    tripSummaryValue = makeLabel("Book a ride first, then start your journey.", 11, false, MUTED);
    tripSummaryValue->setWordWrap(true);
    pl->addWidget(tripSummaryValue);
    auto *actions = new QHBoxLayout;
    auto *start = makeButton("▶  Start Journey");
    auto *finish = makeButton("■  Finish Journey", false);
    actions->addWidget(start); actions->addWidget(finish); actions->addStretch();
    pl->addLayout(actions);
    layout->addWidget(panel); layout->addStretch();
    connect(start, &QPushButton::clicked, this, &MainWindow::startJourney);
    connect(finish, &QPushButton::clicked, this, &MainWindow::finishJourney);
    return page;
}


void MainWindow::navigateTo(int index)
{
    if (pages && index >= 0 && index < pages->count())
        pages->setCurrentIndex(index);
    if (index == 2) refreshHistory();
    if (index == 0) updateDashboard();
}

const Vehicle *MainWindow::selectedVehicle(QString &type) const
{
    type = vehicleInput ? vehicleInput->currentText() : "Mini";
    if (type == "Sedan") { static const Sedan sedan; return &sedan; }
    if (type == "SUV") { static const SUV suv; return &suv; }
    if (type == "Mini") { static const Mini mini; return &mini; }
    return nullptr;
}

double MainWindow::estimateDistance(const QString &pickup, const QString &destination) const
{
    const QString a = pickup.trimmed().toLower();
    const QString b = destination.trimmed().toLower();
    if (a.isEmpty() || b.isEmpty() || a == b) return 0.0;
    quint32 hash = 2166136261u;
    const QString combined = a + "|" + b;
    for (QChar c : combined) { hash ^= c.unicode(); hash *= 16777619u; }
    return 3.0 + static_cast<double>(hash % 1780u) / 100.0;
}

void MainWindow::updateFare()
{
    if (!pickupInput || !destinationInput) return;
    const QString pickup = pickupInput->text().trimmed();
    const QString destination = destinationInput->text().trimmed();
    if (pickup.isEmpty() || destination.isEmpty() || pickup.compare(destination, Qt::CaseInsensitive) == 0) {
        if (fareValue) fareValue->setText("₹0.00");
        if (distanceValue) distanceValue->setText("Estimated distance: —");
        return;
    }
    QString type;
    const Vehicle *vehicle = selectedVehicle(type);
    if (!vehicle) {
        if (fareValue) fareValue->setText("₹0.00");
        if (distanceValue) distanceValue->setText("Estimated distance: —");
        return;
    }
    const double km = estimateDistance(pickup, destination);
    const double fare = vehicle->calculateFare(km);
    if (fareValue) fareValue->setText("₹" + QString::number(fare, 'f', 2));
    if (distanceValue) distanceValue->setText("Estimated distance: " + QString::number(km, 'f', 1) + " km");
}

void MainWindow::bookRide()
{
    try {
        if (!nameInput || !phoneInput || !pickupInput || !destinationInput || !vehicleInput)
            throw std::runtime_error("The booking form is not ready. Please reopen the booking page.");

        const QString passengerName = nameInput->text().trimmed();
        const QString phone = phoneInput->text().trimmed();
        const QString pickup = pickupInput->text().trimmed();
        const QString destination = destinationInput->text().trimmed();

        if (passengerName.isEmpty()) throw std::invalid_argument("Passenger name cannot be empty.");
        if (!QRegularExpression("^[0-9]{10}$").match(phone).hasMatch())
            throw std::invalid_argument("Enter a valid 10-digit phone number.");
        if (pickup.isEmpty()) throw std::invalid_argument("Pickup location cannot be empty.");
        if (destination.isEmpty()) throw std::invalid_argument("Destination cannot be empty.");
        if (pickup.compare(destination, Qt::CaseInsensitive) == 0)
            throw std::invalid_argument("Pickup and destination must be different.");

        QString vehicleType;
        const Vehicle *vehicle = selectedVehicle(vehicleType);
        if (!vehicle) throw std::invalid_argument("Please select a valid vehicle.");

        const double distance = estimateDistance(pickup, destination);
        if (distance <= 0.0) throw std::invalid_argument("The estimated distance must be positive.");
        const double fare = vehicle->calculateFare(distance);
        if (fare <= 0.0) throw std::runtime_error("Unable to calculate the ride fare.");

        Passenger passenger(passengerName, phone);
        Ride newRide(passenger, pickup, destination, vehicleType, distance, fare);
        rides.append(newRide);
saveRides();

activeRideIndex = rides.size() - 1;
        elapsedSeconds = 0;
        if (timerValue) timerValue->setText("00:00:00");
        if (tripSummaryValue) tripSummaryValue->setText("Ride ready to start.\nRoute: " + pickup + " → " + destination);
        refreshHistory();
        updateDashboard();
        updateFare();
        QMessageBox::information(this, "Ride Booked", "Your ride has been booked successfully!\n\nPassenger: " + passengerName + "\nVehicle: " + vehicleType + "\nDistance: " + QString::number(distance, 'f', 1) + " km\nFare: Rs. " + QString::number(fare, 'f', 2));
    }
    catch (const std::invalid_argument &e) {
        QMessageBox::warning(this, "Invalid Input", QString::fromUtf8(e.what()));
    }
    catch (const std::exception &e) {
        QMessageBox::critical(this, "Booking Error", QString::fromUtf8(e.what()));
    }
    catch (...) {
        QMessageBox::critical(this, "Unexpected Error", "An unexpected error occurred while booking the ride.");
    }
}

void MainWindow::updateDashboard()
{
    if (!pages || pages->count() == 0) return;
    QWidget *dashboard = pages->widget(0);
    if (!dashboard) return;
    if (auto *value = dashboard->findChild<QLabel *>("totalBookingsValue"))
        value->setText(QString::number(rides.size()));
    if (auto *value = dashboard->findChild<QLabel *>("currentTripValue")) {
        if (activeRideIndex >= 0 && activeRideIndex < rides.size())
            value->setText(rides[activeRideIndex].getStatus());
        else value->setText("No active ride");
    }
    if (auto *value = dashboard->findChild<QLabel *>("latestDistanceValue")) {
        if (!rides.isEmpty()) value->setText(QString::number(rides.last().getDistance(), 'f', 1) + " km");
        else value->setText("—");
    }
}

void MainWindow::refreshHistory()
{
    if (!historyTable) return;
    historyTable->setRowCount(0);
    for (int i = 0; i < rides.size(); ++i) {
        const Ride &ride = rides[i];
        const int row = historyTable->rowCount();
        historyTable->insertRow(row);
        auto *dateItem = new QTableWidgetItem(ride.getBookedAt().toString("dd MMM yyyy, hh:mm AP"));
        dateItem->setData(Qt::UserRole, i);
        historyTable->setItem(row, 0, dateItem);
        historyTable->setItem(row, 1, new QTableWidgetItem(ride.getPassengerName()));
        historyTable->setItem(row, 2, new QTableWidgetItem(ride.getPickup() + " → " + ride.getDestination()));
        historyTable->setItem(row, 3, new QTableWidgetItem(ride.getVehicleType()));
        historyTable->setItem(row, 4, new QTableWidgetItem("Rs. " + QString::number(ride.getFare(), 'f', 2)));
        historyTable->setItem(row, 5, new QTableWidgetItem(ride.getStatus()));
    }
}

void MainWindow::showRideDetails(int index)
{
    if (index < 0 || index >= rides.size()) {
        QMessageBox::warning(this, "Invalid Selection", "The selected ride could not be found.");
        return;
    }
    QMessageBox::information(this, "Ride Details", rides[index].getSummary());
}
QString MainWindow::storageFilePath() const
{
    const QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);

    QDir().mkpath(folder);

    return folder + "/rides.json";
}

void MainWindow::saveRides()
{
    QJsonArray ridesArray;

    for (const Ride &ride : rides)
    {
        QJsonObject obj;

        obj["name"] = ride.getPassengerName();
        obj["phone"] = ride.getPhone();
        obj["pickup"] = ride.getPickup();
        obj["destination"] = ride.getDestination();
        obj["vehicle"] = ride.getVehicleType();
        obj["distance"] = ride.getDistance();
        obj["fare"] = ride.getFare();
        obj["status"] = ride.getStatus();
        obj["bookedAt"] =
            ride.getBookedAt().toString(Qt::ISODate);

        ridesArray.append(obj);
    }

    QSaveFile file(storageFilePath());

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Could not save rides:"
                   << file.errorString();
        return;
    }

    file.write(
        QJsonDocument(ridesArray)
            .toJson(QJsonDocument::Indented));

    if (!file.commit())
    {
        qWarning() << "Failed to save ride history:"
                   << file.errorString();
    }
}

void MainWindow::loadRides()
{
    QFile file(storageFilePath());

    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not load rides:"
                   << file.errorString();
        return;
    }

    QJsonParseError parseError;

    const QJsonDocument document =
        QJsonDocument::fromJson(
            file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError ||
        !document.isArray())
    {
        qWarning() << "Invalid saved ride data:"
                   << parseError.errorString();
        return;
    }

    rides.clear();
    activeRideIndex = -1;

    for (const QJsonValue &value : document.array())
    {
        if (!value.isObject())
            continue;

        const QJsonObject obj = value.toObject();

        Passenger passenger(
            obj["name"].toString(),
            obj["phone"].toString());

        const QString pickup =
            obj["pickup"].toString();

        const QString destination =
            obj["destination"].toString();

        const QString vehicle =
            obj["vehicle"].toString();

        const double distance =
            obj["distance"].toDouble();

        const double fare =
            obj["fare"].toDouble();

        QString status =
            obj["status"].toString("Booked");

        QDateTime bookedAt =
            QDateTime::fromString(
                obj["bookedAt"].toString(),
                Qt::ISODate);

        if (!bookedAt.isValid())
            bookedAt = QDateTime::currentDateTime();

        // Do not resume a journey that was interrupted
        // when the application was closed.
        if (status == "In Progress")
            status = "Booked";

        Ride ride(
            passenger,
            pickup,
            destination,
            vehicle,
            distance,
            fare,
            status,
            bookedAt);

        rides.append(ride);
    }
}

void MainWindow::cancelSelectedRide()
{
    if (!historyTable || historyTable->currentRow() < 0) {
        QMessageBox::information(this, "Select a ride", "Select a ride in the table first.");
        return;
    }
    const int row = historyTable->currentRow();
    const int index = historyTable->item(row, 0)->data(Qt::UserRole).toInt();
    if (index < 0 || index >= rides.size()) return;
    if (rides[index].getStatus() != "Booked") {
        QMessageBox::information(this, "Cannot cancel", "Only a booked ride can be cancelled.");
        return;
    }
    if (QMessageBox::question(this, "Cancel ride", "Cancel the selected ride?") != QMessageBox::Yes) return;
    rides[index].setStatus("Cancelled");
    saveRides();
    if (activeRideIndex == index) {
        activeRideIndex = -1;
        journeyTimer->stop();
        elapsedSeconds = 0;
        if (timerValue) timerValue->setText("00:00:00");
        if (tripSummaryValue) tripSummaryValue->setText("Ride cancelled. Book another ride to continue.");
    }
    refreshHistory();
    updateDashboard();
    QMessageBox::information(this, "Ride cancelled", "The ride status is now Cancelled.");
}

void MainWindow::startJourney()
{
    if (activeRideIndex < 0 || activeRideIndex >= rides.size()) {
        QMessageBox::information(this, "No booked ride", "Book a ride before starting a journey.");
        return;
    }
    Ride &ride = rides[activeRideIndex];
    if (ride.getStatus() != "Booked") {
        QMessageBox::information(this, "Cannot start", "Only a booked ride can be started.");
        return;
    }
    ride.setStatus("In Progress");
    saveRides();
    elapsedSeconds = 0;
    journeyTimer->start();
    if (timerValue) timerValue->setText("00:00:00");
    if (tripSummaryValue) tripSummaryValue->setText("Route: " + ride.getPickup() + " → " + ride.getDestination() + "\nVehicle: " + ride.getVehicleType() + "\nEstimated distance: " + QString::number(ride.getDistance(), 'f', 1) + " km");
    refreshHistory();
    updateDashboard();
}

void MainWindow::finishJourney()
{
    if (activeRideIndex < 0 || activeRideIndex >= rides.size() || rides[activeRideIndex].getStatus() != "In Progress") {
        QMessageBox::information(this, "No active journey", "Start a booked journey first.");
        return;
    }
    journeyTimer->stop();
    rides[activeRideIndex].setStatus("Completed");
    saveRides();
    const Ride &ride = rides[activeRideIndex];
    const QString summary = "Journey completed!\n\nRoute: " + ride.getPickup() + " → " + ride.getDestination() + "\nDuration: " + (timerValue ? timerValue->text() : QString("00:00:00")) + "\nEstimated distance: " + QString::number(ride.getDistance(), 'f', 1) + " km\nFare: Rs. " + QString::number(ride.getFare(), 'f', 2);
    if (tripSummaryValue) tripSummaryValue->setText(summary);
    refreshHistory();
    updateDashboard();
    QMessageBox::information(this, "Trip summary", summary);
}

void MainWindow::updateJourneyTimer()
{
    ++elapsedSeconds;
    const int hours = elapsedSeconds / 3600;
    const int minutes = (elapsedSeconds % 3600) / 60;
    const int seconds = elapsedSeconds % 60;
    if (timerValue) timerValue->setText(QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    if (tripSummaryValue && activeRideIndex >= 0 && activeRideIndex < rides.size()) {
        const Ride &r = rides[activeRideIndex];
        tripSummaryValue->setText("LIVE JOURNEY\nRoute: " + r.getPickup() + " → " + r.getDestination() + "\nVehicle: " + r.getVehicleType() + "\nEstimated distance: " + QString::number(r.getDistance(), 'f', 1) + " km");
    }
}
