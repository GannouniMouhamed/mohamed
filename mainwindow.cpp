#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QDate>
#include <QPainter>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QSpinBox>
#include <QDateEdit>
#include <QTableWidgetItem>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionValidator>
#include <QDoubleValidator>
#include <QCheckBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QGroupBox>
#include <QRadioButton>
#include <QProcess>
#include <QApplication>
#include <QClipboard>
#include <QMap>
#include <QTimer>
#include <QTime>
#include <cmath>
#include <QSplitter>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QFont>
#include <QFontMetrics>
#include <QtMath>

// =======================
// PieChartWidget (implementation)
// =======================
PieChartWidget::PieChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_total(0)
{
    m_colors << QColor(92, 122, 58)   << QColor(76, 175, 80)
             << QColor(106, 138, 74)  << QColor(139, 195, 74)
             << QColor(33, 150, 243)  << QColor(255, 152, 0)
             << QColor(156, 39, 176)  << QColor(121, 85, 72);
}

void PieChartWidget::setData(const QMap<QString, int> &data, int total)
{
    m_data = data;
    m_total = total;
    update();
}

void PieChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_total == 0 || m_data.isEmpty()) {
        painter.setPen(QColor(74, 106, 42));
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "Aucune production enregistrée.");
        return;
    }

    QRect chartRect = rect();
    int margin = 10;
    int legendWidth = 160;
    int legendItemHeight = 24;
    int maxChartSize = qMin(chartRect.width() - legendWidth - margin * 3,
                            chartRect.height() - margin * 2 - 30);
    int chartX = margin;
    int chartY = margin;
    int chartSize = maxChartSize;
    int legendStartY = chartY;
    QRect pieRect(chartX, chartY, chartSize, chartSize);

    double startAngle = 0;
    int colorIndex = 0;
    int dataIndex = 0;

    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        double percentage = (it.value() * 360.0) / m_total;
        double spanAngle = percentage;
        QColor color = m_colors[colorIndex % m_colors.size()];
        painter.setBrush(color);
        painter.setPen(QPen(QColor(255, 255, 255), 2));
        painter.drawPie(pieRect, static_cast<int>(startAngle * 16), static_cast<int>(spanAngle * 16));

        int legendX = chartX + chartSize + margin;
        int legendY = legendStartY + dataIndex * legendItemHeight;
        QRect colorBox(legendX, legendY, 15, 15);
        painter.setBrush(color);
        painter.setPen(QPen(QColor(200, 200, 200), 1));
        painter.drawRect(colorBox);

        painter.setPen(QColor(26, 48, 9));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QString label = QString("%1 (%2%)")
                           .arg(it.key())
                           .arg(QString::number((it.value() * 100.0) / m_total, 'f', 1));
        painter.drawText(legendX + 20, legendY + 12, label);

        startAngle += spanAngle;
        colorIndex++;
        dataIndex++;
    }

    painter.setPen(QColor(74, 106, 42));
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    QString totalText = QString("Total: %1 production%2").arg(m_total).arg(m_total > 1 ? "s" : "");
    QFontMetrics fm(painter.font());
    int textWidth = fm.horizontalAdvance(totalText);
    int textY = chartY + chartSize + 20;
    if (textY < chartRect.height() - 5) {
        painter.drawText((chartRect.width() - textWidth) / 2, textY, totalText);
    }
}

// =======================
// AnimatedBackgroundWidget
// =======================
class AnimatedBackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnimatedBackgroundWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_NoSystemBackground, false);
        setAutoFillBackground(false);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&AnimatedBackgroundWidget::update));
        timer->start(30);
    }

    void initializeOlives(int w, int h)
    {
        olives.clear();
        for (int i = 0; i < 15; ++i) {
            olives.append({
                QPointF(rand() % qMax(1, w), rand() % qMax(1, h)),
                QPointF((rand() % 200 - 100) / 50.0, (rand() % 200 - 100) / 50.0),
                (rand() % 30 + 20) / 10.0,
                QColor(85 + rand() % 30, 107 + rand() % 30, 47 + rand() % 20)
            });
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int w = width();
        int h = height();

        QLinearGradient gradient(0, 0, w, h);
        gradient.setColorAt(0, QColor(85, 107, 47));
        gradient.setColorAt(0.3, QColor(107, 142, 35));
        gradient.setColorAt(0.6, QColor(128, 128, 0));
        gradient.setColorAt(1, QColor(85, 107, 47));
        painter.fillRect(rect(), gradient);

        for (auto &olive : olives) {
            olive.pos += olive.velocity;

            if (olive.pos.x() < 0 || olive.pos.x() > w) {
                olive.velocity.setX(-olive.velocity.x());
            }
            if (olive.pos.y() < 0 || olive.pos.y() > h) {
                olive.velocity.setY(-olive.velocity.y());
            }

            olive.pos.setX(qBound(0.0, olive.pos.x(), double(w)));
            olive.pos.setY(qBound(0.0, olive.pos.y(), double(h)));

            painter.setBrush(QBrush(olive.color));
            painter.setPen(QPen(QColor(olive.color.red() - 20, olive.color.green() - 20, olive.color.blue() - 20), 2));

            int size = int(olive.size * 15);
            QRectF oliveRect(olive.pos.x() - size/2, olive.pos.y() - size/2, size, size * 1.3);
            painter.drawEllipse(oliveRect);

            painter.setBrush(QBrush(QColor(255, 255, 255, 80)));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(oliveRect.adjusted(size * 0.2, size * 0.2, -size * 0.6, -size * 0.4));
        }
    }

    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        int w = width();
        int h = height();
        for (auto &olive : olives) {
            if (olive.pos.x() > w) olive.pos.setX(w - 50);
            if (olive.pos.y() > h) olive.pos.setY(h - 50);
            if (olive.pos.x() < 0) olive.pos.setX(50);
            if (olive.pos.y() < 0) olive.pos.setY(50);
        }
    }

    void showEvent(QShowEvent *event) override
    {
        QWidget::showEvent(event);
        if (parentWidget()) {
            int w = parentWidget()->width();
            int h = parentWidget()->height();
            setGeometry(0, 0, w, h);
            if (olives.isEmpty()) {
                initializeOlives(w, h);
            }
        }
    }

private:
    struct Olive {
        QPointF pos;
        QPointF velocity;
        double size;
        QColor color;
    };

    QVector<Olive> olives;
    QTimer *timer;
};

#include "mainwindow.moc"

// =======================
// Quiz Questions
// =======================

struct QuizQuestion {
    QString question;
    QStringList options;
    int correctIndex;
    QString explanation;
};

static QVector<QuizQuestion> createOliveraQuiz()
{
    return {
        {
            "Quel est l'objectif principal\nde l'application OLIVERAQ ?",
            {
                "Gérer uniquement la paie des employés d'une presse",
                "Digitaliser et centraliser la gestion complète\nd'une presse d'huile d'olive",
                "Vendre de l'huile d'olive en ligne au grand public"
            },
            1,
            "OLIVERAQ est une solution de **gestion intégrée** pour les presses d'huile d'olive : production, employés, stocks et flux commerciaux."
        },
        {
            "Qu'apporte OLIVERAQ\nen termes de traçabilité ?",
            {
                "Aucune, c'est une simple interface graphique",
                "Une traçabilité partielle uniquement des paiements",
                "Une traçabilité complète des extractions d'huile\net des opérations associées"
            },
            2,
            "OLIVERAQ permet d'assurer une **traçabilité complète** des extractions et des opérations liées."
        },
        {
            "Comment OLIVERAQ aide\nla prise de décision ?",
            {
                "Grâce à des statistiques et\nrapports détaillés sur l'activité",
                "En envoyant des SMS aux clients automatiquement",
                "En remplaçant totalement le travail de l'opérateur"
            },
            0,
            "L'application fournit des **statistiques** et **rapports détaillés** pour mieux piloter l'activité."
        },
        {
            "Quel module pédagogique est intégré dans OLIVERAQ ?",
            {
                "Un module de jeux vidéo",
                "Un module de quiz pédagogique",
                "Un module de comptabilité avancée"
            },
            1,
            "Un **module de quiz pédagogique** permet aux utilisateurs de se former sur OLIVERAQ et sur la gestion des presses."
        },
        {
            "Que peut faire le chatbot intelligent d'OLIVERAQ ?",
            {
                "Uniquement traduire des textes",
                "Saluer l'utilisateur, répondre à ses questions et le guider dans l'application",
                "Gérer automatiquement les transactions bancaires"
            },
            1,
            "Le chatbot est conçu pour **accompagner l'utilisateur** : accueil, réponses aux questions fréquentes et aide au parcours dans l'application."
        }
    };
}

// =======================
// MainWindow
// =======================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupStyle();

    resize(1300, 750);
    setMinimumSize(1150, 650);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupStyle()
{
    QString style = R"(
        QWidget {
            background-color: #f4f7f1;
        }
        QPushButton {
            background-color: #7a8f4e;
            color: white;
            padding: 8px;
            border-radius: 6px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #9bb35d;
        }
        QLabel#titleLabel {
            font-size: 20px;
            font-weight: bold;
            color: #5f6f3e;
        }
        QFrame#menuFrame {
            background-color: #5f6f3e;
        }
        QLabel#label {
            font-weight: bold;
        }
        QLineEdit[readOnly="true"] {
            background-color: #f0f0f0;
        }
    )";
    this->setStyleSheet(style);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    mainStack = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(mainStack);

    // ======================================
    // PAGE LOGIN
    // ======================================
    pageLogin = new QWidget(this);

    QVBoxLayout *loginLayout = new QVBoxLayout(pageLogin);
    loginLayout->setContentsMargins(0, 0, 0, 0);
    loginLayout->setSpacing(0);
    loginLayout->setAlignment(Qt::AlignCenter);

    AnimatedBackgroundWidget *animatedBg = new AnimatedBackgroundWidget(pageLogin);
    animatedBg->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    animatedBg->lower();

    QTimer *resizeTimer = new QTimer(pageLogin);
    QWidget *loginPage = pageLogin;
    connect(resizeTimer, &QTimer::timeout, [animatedBg, loginPage]() {
        if (loginPage && animatedBg) {
            int w = loginPage->width();
            int h = loginPage->height();
            if (animatedBg->width() != w || animatedBg->height() != h) {
                animatedBg->setGeometry(0, 0, w, h);
                if (animatedBg->width() > 0 && animatedBg->height() > 0) {
                    animatedBg->initializeOlives(w, h);
                }
            }
        }
    });
    resizeTimer->start(100);

    QTimer::singleShot(200, [animatedBg, loginPage]() {
        if (loginPage && animatedBg) {
            animatedBg->setGeometry(0, 0, loginPage->width(), loginPage->height());
            animatedBg->initializeOlives(loginPage->width(), loginPage->height());
        }
    });

    QWidget *loginContainer = new QWidget(pageLogin);
    loginContainer->setFixedWidth(450);
    loginContainer->setStyleSheet(
        "QWidget { "
        "background-color: #ffffff; "
        "border: none; "
        "border-radius: 12px; "
        "}"
        );

    QVBoxLayout *containerLayout = new QVBoxLayout(loginContainer);
    containerLayout->setSpacing(0);
    containerLayout->setContentsMargins(50, 60, 50, 60);

    QLabel *loginTitle = new QLabel("Connexion");
    loginTitle->setObjectName("loginTitle");
    loginTitle->setAlignment(Qt::AlignCenter);
    loginTitle->setStyleSheet(
        "QLabel { "
        "font-size: 28px; "
        "font-weight: 600; "
        "color: #556b2f; "
        "margin-bottom: 8px; "
        "letter-spacing: 0.5px; "
        "}"
        );

    QLabel *loginSubtitle = new QLabel("Accédez à votre espace de gestion");
    loginSubtitle->setAlignment(Qt::AlignCenter);
    loginSubtitle->setStyleSheet(
        "QLabel { "
        "font-size: 13px; "
        "color: #6b8e23; "
        "margin-bottom: 35px; "
        "font-weight: 400; "
        "}"
        );

    QLabel *labelUser = new QLabel("Nom d'utilisateur");
    labelUser->setStyleSheet(
        "QLabel { "
        "font-size: 12px; "
        "color: #556b2f; "
        "font-weight: 500; "
        "margin-bottom: 5px; "
        "}"
        );

    editUser = new QLineEdit(loginContainer);
    editUser->setPlaceholderText("Entrez votre nom d'utilisateur");
    editUser->setFixedHeight(42);
    editUser->setStyleSheet(
        "QLineEdit { "
        "font-size: 14px; "
        "padding: 10px 12px; "
        "border: 1px solid #c0c0c0; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "}"
        "QLineEdit:focus { "
        "border: 2px solid #6b8e23; "
        "outline: none; "
        "background-color: #fafff0; "
        "}"
        "QLineEdit:hover { "
        "border: 1px solid #9aab6b; "
        "}"
        );

    QLabel *labelPass = new QLabel("Mot de passe");
    labelPass->setStyleSheet(
        "QLabel { "
        "font-size: 12px; "
        "color: #556b2f; "
        "font-weight: 500; "
        "margin-top: 15px; "
        "margin-bottom: 5px; "
        "}"
        );

    editPass = new QLineEdit(loginContainer);
    editPass->setPlaceholderText("Entrez votre mot de passe");
    editPass->setEchoMode(QLineEdit::Password);
    editPass->setFixedHeight(42);
    editPass->setStyleSheet(
        "QLineEdit { "
        "font-size: 14px; "
        "padding: 10px 12px; "
        "border: 1px solid #c0c0c0; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "}"
        "QLineEdit:focus { "
        "border: 2px solid #6b8e23; "
        "outline: none; "
        "background-color: #fafff0; "
        "}"
        "QLineEdit:hover { "
        "border: 1px solid #9aab6b; "
        "}"
        );

    btnLogin = new QPushButton("Se connecter", loginContainer);
    btnLogin->setFixedHeight(44);
    btnLogin->setStyleSheet(
        "QPushButton { "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #ffffff; "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #6b8e23, "
        "stop:1 #556b2f); "
        "border: none; "
        "border-radius: 6px; "
        "padding: 11px; "
        "letter-spacing: 0.3px; "
        "}"
        "QPushButton:hover { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #7a9e2d, "
        "stop:1 #6b8e23); "
        "}"
        "QPushButton:pressed { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #556b2f, "
        "stop:1 #4a5a26); "
        "}"
        );

    containerLayout->addWidget(loginTitle);
    containerLayout->addWidget(loginSubtitle);
    containerLayout->addWidget(labelUser);
    containerLayout->addWidget(editUser);
    containerLayout->addWidget(labelPass);
    containerLayout->addWidget(editPass);
    containerLayout->addSpacing(25);
    containerLayout->addWidget(btnLogin);

    loginLayout->addWidget(loginContainer);

    mainStack->addWidget(pageLogin);

    connect(btnLogin, &QPushButton::clicked, this, [=]() {
        mainStack->setCurrentWidget(pageMenu);
    });

    // ======================================
    // PAGE MENU
    // ======================================
    pageMenu = new QWidget(this);
    pageMenu->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #556b2f, "
        "stop:0.25 #6b8e23, "
        "stop:0.5 #808000, "
        "stop:0.75 #6b8e23, "
        "stop:1 #556b2f); "
        "}"
        );

    QVBoxLayout *menuLayout = new QVBoxLayout(pageMenu);
    menuLayout->setContentsMargins(0, 0, 0, 0);
    menuLayout->setSpacing(0);
    menuLayout->setAlignment(Qt::AlignTop);

    QWidget *headerWidget = new QWidget(pageMenu);
    headerWidget->setFixedHeight(80);
    headerWidget->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #556b2f, "
        "stop:1 #6b8e23); "
        "border-bottom: 3px solid #4a5a26; "
        "}"
        );

    QVBoxLayout *menuHeaderLayout = new QVBoxLayout(headerWidget);
    menuHeaderLayout->setContentsMargins(40, 0, 40, 0);
    menuHeaderLayout->setSpacing(0);

    QLabel *menuTitle = new QLabel("Menu Principal");
    menuTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    menuTitle->setStyleSheet(
        "QLabel { "
        "font-size: 24px; "
        "font-weight: 600; "
        "color: #ffffff; "
        "letter-spacing: 0.5px; "
        "text-shadow: 1px 1px 2px rgba(0,0,0,0.3); "
        "}"
        );

    menuHeaderLayout->addWidget(menuTitle);

    QWidget *buttonsContainer = new QWidget(pageMenu);
    buttonsContainer->setStyleSheet(
        "QWidget { "
        "background-color: #ffffff; "
        "}"
        );

    QVBoxLayout *buttonsLayout = new QVBoxLayout(buttonsContainer);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(40, 30, 40, 30);

    btnGestionEmployes   = new QPushButton("Gestion Employés", buttonsContainer);
    btnGestionClients    = new QPushButton("Gestion Commandes Clients", buttonsContainer);
    btnGestionStocks     = new QPushButton("Gestion Stocks", buttonsContainer);
    btnGestionVentes     = new QPushButton("Gestion Ventes", buttonsContainer);
    btnFournisseur       = new QPushButton("Gestion Commandes Fournisseurs", buttonsContainer);
    btnModifMotDePasse   = new QPushButton("Modifier mot de passe", buttonsContainer);
    btnQuiz              = new QPushButton("🎓 Quiz pédagogique OLIVERAQ", buttonsContainer);

    QString buttonStyle =
        "QPushButton { "
        "font-size: 14px; "
        "font-weight: 500; "
        "color: #2c3e50; "
        "background-color: #ffffff; "
        "border: 1px solid #e0e0e0; "
        "border-left: 4px solid #ecf0f1; "
        "border-radius: 0px; "
        "padding: 16px 20px; "
        "text-align: left; "
        "min-height: 50px; "
        "}"
        "QPushButton:hover { "
        "background-color: #fafff0; "
        "border-left: 4px solid #6b8e23; "
        "color: #556b2f; "
        "}"
        "QPushButton:pressed { "
        "background-color: #f0f5e0; "
        "border-left: 4px solid #556b2f; "
        "color: #4a5a26; "
        "}";

    btnGestionEmployes->setStyleSheet(buttonStyle);
    btnGestionClients->setStyleSheet(buttonStyle);
    btnGestionStocks->setStyleSheet(buttonStyle);
    btnGestionVentes->setStyleSheet(buttonStyle);
    btnFournisseur->setStyleSheet(buttonStyle);
    btnModifMotDePasse->setStyleSheet(buttonStyle);
    btnQuiz->setStyleSheet(buttonStyle);

    buttonsLayout->addWidget(btnGestionEmployes);
    buttonsLayout->addWidget(btnGestionClients);
    buttonsLayout->addWidget(btnGestionStocks);
    buttonsLayout->addWidget(btnGestionVentes);
    buttonsLayout->addWidget(btnFournisseur);
    buttonsLayout->addWidget(btnModifMotDePasse);
    buttonsLayout->addWidget(btnQuiz);

    menuLayout->addWidget(headerWidget);
    menuLayout->addWidget(buttonsContainer);
    menuLayout->addStretch();

    mainStack->addWidget(pageMenu);

    connect(btnGestionEmployes, &QPushButton::clicked, this, &MainWindow::openGestionEmployes);
    connect(btnGestionClients, &QPushButton::clicked, this, &MainWindow::openGestionClients);
    connect(btnGestionStocks, &QPushButton::clicked, this, &MainWindow::openGestionStocks);
    connect(btnModifMotDePasse, &QPushButton::clicked, this, &MainWindow::openChangePassword);
    connect(btnFournisseur, &QPushButton::clicked, this, &MainWindow::openGestionFournisseurs);
    connect(btnQuiz, &QPushButton::clicked, this, &MainWindow::openQuiz);

    // ======================================
    // PAGE CHANGEMENT MOT DE PASSE
    // ======================================
    pageChangePassword = new QWidget(this);
    pageChangePassword->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #556b2f, "
        "stop:0.25 #6b8e23, "
        "stop:0.5 #808000, "
        "stop:0.75 #6b8e23, "
        "stop:1 #556b2f); "
        "}"
        );

    QVBoxLayout *passLayout = new QVBoxLayout(pageChangePassword);
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(0);
    passLayout->setAlignment(Qt::AlignTop);

    QWidget *passHeaderWidget = new QWidget(pageChangePassword);
    passHeaderWidget->setFixedHeight(80);
    passHeaderWidget->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #556b2f, "
        "stop:1 #6b8e23); "
        "border-bottom: 3px solid #4a5a26; "
        "}"
        );

    QVBoxLayout *passHeaderLayout = new QVBoxLayout(passHeaderWidget);
    passHeaderLayout->setContentsMargins(40, 0, 40, 0);
    passHeaderLayout->setSpacing(0);

    QLabel *passTitle = new QLabel("Modifier mot de passe");
    passTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    passTitle->setStyleSheet(
        "QLabel { "
        "font-size: 24px; "
        "font-weight: 600; "
        "color: #ffffff; "
        "letter-spacing: 0.5px; "
        "text-shadow: 1px 1px 2px rgba(0,0,0,0.3); "
        "}"
        );

    passHeaderLayout->addWidget(passTitle);

    QWidget *passContainer = new QWidget(pageChangePassword);
    passContainer->setStyleSheet(
        "QWidget { "
        "background-color: #ffffff; "
        "}"
        );

    QVBoxLayout *passContainerLayout = new QVBoxLayout(passContainer);
    passContainerLayout->setContentsMargins(60, 50, 60, 50);
    passContainerLayout->setSpacing(20);
    passContainerLayout->setAlignment(Qt::AlignCenter);

    QLabel *labelOldPass = new QLabel("Ancien mot de passe");
    labelOldPass->setStyleSheet(
        "QLabel { "
        "font-size: 13px; "
        "color: #556b2f; "
        "font-weight: 500; "
        "margin-bottom: 5px; "
        "}"
        );

    editOldPass = new QLineEdit(passContainer);
    editOldPass->setPlaceholderText("Entrez votre ancien mot de passe");
    editOldPass->setEchoMode(QLineEdit::Password);
    editOldPass->setFixedHeight(42);
    editOldPass->setStyleSheet(
        "QLineEdit { "
        "font-size: 14px; "
        "padding: 10px 12px; "
        "border: 1px solid #d5d5d5; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "}"
        "QLineEdit:focus { "
        "border: 2px solid #6b8e23; "
        "outline: none; "
        "background-color: #fafff0; "
        "}"
        "QLineEdit:hover { "
        "border: 1px solid #9aab6b; "
        "}"
        );

    QLabel *labelNewPass = new QLabel("Nouveau mot de passe");
    labelNewPass->setStyleSheet(
        "QLabel { "
        "font-size: 13px; "
        "color: #556b2f; "
        "font-weight: 500; "
        "margin-top: 10px; "
        "margin-bottom: 5px; "
        "}"
        );

    editNewPass = new QLineEdit(passContainer);
    editNewPass->setPlaceholderText("Entrez votre nouveau mot de passe");
    editNewPass->setEchoMode(QLineEdit::Password);
    editNewPass->setFixedHeight(42);
    editNewPass->setStyleSheet(
        "QLineEdit { "
        "font-size: 14px; "
        "padding: 10px 12px; "
        "border: 1px solid #d5d5d5; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "}"
        "QLineEdit:focus { "
        "border: 2px solid #6b8e23; "
        "outline: none; "
        "background-color: #fafff0; "
        "}"
        "QLineEdit:hover { "
        "border: 1px solid #9aab6b; "
        "}"
        );

    QLabel *labelConfirmPass = new QLabel("Confirmer le mot de passe");
    labelConfirmPass->setStyleSheet(
        "QLabel { "
        "font-size: 13px; "
        "color: #556b2f; "
        "font-weight: 500; "
        "margin-top: 10px; "
        "margin-bottom: 5px; "
        "}"
        );

    editConfirmPass = new QLineEdit(passContainer);
    editConfirmPass->setPlaceholderText("Confirmez votre nouveau mot de passe");
    editConfirmPass->setEchoMode(QLineEdit::Password);
    editConfirmPass->setFixedHeight(42);
    editConfirmPass->setStyleSheet(
        "QLineEdit { "
        "font-size: 14px; "
        "padding: 10px 12px; "
        "border: 1px solid #d5d5d5; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "}"
        "QLineEdit:focus { "
        "border: 2px solid #6b8e23; "
        "outline: none; "
        "background-color: #fafff0; "
        "}"
        "QLineEdit:hover { "
        "border: 1px solid #9aab6b; "
        "}"
        );

    QHBoxLayout *btnPassLayout = new QHBoxLayout();
    btnPassLayout->setSpacing(15);

    btnSavePass = new QPushButton("Sauvegarder", passContainer);
    btnSavePass->setFixedHeight(44);
    btnSavePass->setStyleSheet(
        "QPushButton { "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #ffffff; "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #6b8e23, "
        "stop:1 #556b2f); "
        "border: none; "
        "border-radius: 6px; "
        "padding: 11px 30px; "
        "letter-spacing: 0.3px; "
        "min-width: 150px; "
        "}"
        "QPushButton:hover { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #7a9e2d, "
        "stop:1 #6b8e23); "
        "}"
        "QPushButton:pressed { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #556b2f, "
        "stop:1 #4a5a26); "
        "}"
        );

    btnBackPass = new QPushButton("Retour", passContainer);
    btnBackPass->setFixedHeight(44);
    btnBackPass->setStyleSheet(
        "QPushButton { "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #556b2f; "
        "background-color: #ffffff; "
        "border: 2px solid #6b8e23; "
        "border-radius: 6px; "
        "padding: 11px 30px; "
        "letter-spacing: 0.3px; "
        "min-width: 150px; "
        "}"
        "QPushButton:hover { "
        "background-color: #fafff0; "
        "border: 2px solid #556b2f; "
        "color: #4a5a26; "
        "}"
        "QPushButton:pressed { "
        "background-color: #f0f5e0; "
        "}"
        );

    btnPassLayout->addStretch();
    btnPassLayout->addWidget(btnBackPass);
    btnPassLayout->addWidget(btnSavePass);
    btnPassLayout->addStretch();

    passContainerLayout->addStretch();
    passContainerLayout->addWidget(labelOldPass);
    passContainerLayout->addWidget(editOldPass);
    passContainerLayout->addWidget(labelNewPass);
    passContainerLayout->addWidget(editNewPass);
    passContainerLayout->addWidget(labelConfirmPass);
    passContainerLayout->addWidget(editConfirmPass);
    passContainerLayout->addSpacing(30);
    passContainerLayout->addLayout(btnPassLayout);
    passContainerLayout->addStretch();

    passLayout->addWidget(passHeaderWidget);
    passLayout->addWidget(passContainer);
    passLayout->addStretch();

    mainStack->addWidget(pageChangePassword);

    connect(btnSavePass, &QPushButton::clicked, this, &MainWindow::changePassword);
    connect(btnBackPass, &QPushButton::clicked, this, &MainWindow::backToMenu);

    // ======================================
    // PAGE EMPLOYES
    // ======================================
    pageEmployes = new QWidget(this);
    QHBoxLayout *empLayout = new QHBoxLayout(pageEmployes);

    // LEFT SIDE (LISTE)
    QWidget *left = new QWidget(pageEmployes);
    QVBoxLayout *leftLayout = new QVBoxLayout(left);
    leftLayout->setSpacing(0);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *headerEmp = new QWidget(left);
    headerEmp->setFixedHeight(80);
    headerEmp->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #556b2f, stop:1 #6b8e23); "
        "border-bottom: 3px solid #4a5a26; "
        "}"
        );

    QHBoxLayout *headerLayout = new QHBoxLayout(headerEmp);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    headerLayout->setSpacing(10);

    QLabel *titleEmp = new QLabel("📋 EMPLOYÉS", headerEmp);
    titleEmp->setStyleSheet("color:white; font-size:18px; font-weight:bold;");
    titleEmp->setAlignment(Qt::AlignCenter);

    editSearch = new QLineEdit(headerEmp);
    editSearch->setPlaceholderText("Rechercher par nom...");
    editSearch->setFixedHeight(40);
    editSearch->setFixedWidth(250);
    editSearch->setStyleSheet(
        "QLineEdit { "
        "font-size: 13px; "
        "padding: 8px 12px; "
        "border: 1px solid #d5d5d5; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "margin-right: 10px; "
        "}"
        "QLineEdit:focus { "
        "border: 2px solid #6b8e23; "
        "outline: none; "
        "}"
        );

    QPushButton *btnSearchEmp = new QPushButton("🔍", headerEmp);
    btnSearchEmp->setFixedSize(40, 40);
    btnSearchEmp->setStyleSheet(
        "QPushButton { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #6b8e23, stop:1 #556b2f); "
        "color: white; "
        "font-size: 16px; "
        "border: none; "
        "border-radius: 6px; "
        "font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #7a9e2d, stop:1 #6b8e23); "
        "}"
        );

    comboSort = new QComboBox(headerEmp);
    comboSort->addItem("Trier par salaire (croissant)");
    comboSort->addItem("Trier par salaire (décroissant)");
    comboSort->setFixedHeight(40);
    comboSort->setFixedWidth(280);
    comboSort->setStyleSheet(
        "QComboBox { "
        "font-size: 13px; "
        "padding: 8px 12px; "
        "border: 1px solid #d5d5d5; "
        "border-radius: 6px; "
        "background-color: #ffffff; "
        "color: #2c3e50; "
        "margin-right: 10px; "
        "}"
        "QComboBox:hover { "
        "border: 2px solid #6b8e23; "
        "}"
        "QComboBox::drop-down { border: none; }"
        );

    headerLayout->addWidget(titleEmp);
    headerLayout->addStretch();
    headerLayout->addWidget(editSearch);
    headerLayout->addWidget(btnSearchEmp);
    headerLayout->addWidget(comboSort);

    leftLayout->addWidget(headerEmp);

    tableEmployes = new QTableWidget(left);
    tableEmployes->setColumnCount(10);
    tableEmployes->setHorizontalHeaderLabels(
        {"ID","Nom","Prénom","Poste","Email","Téléphone","Salaire","Heures","Date embauche","Date naissance"});
    tableEmployes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableEmployes->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableEmployes->setAlternatingRowColors(true);
    tableEmployes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableEmployes->setStyleSheet(
        "QTableWidget { "
        "gridline-color: #e0e0e0; "
        "background-color: #ffffff; "
        "border: 1px solid #e0e0e0; "
        "border-radius: 6px; "
        "}"
        "QTableWidget::item { padding: 8px; } "
        "QHeaderView::section { "
        "background-color: #f5f7fa; "
        "color: #556b2f; "
        "font-weight: 600; "
        "padding: 10px; "
        "border: 1px solid #e0e0e0; "
        "}"
        );

    leftLayout->addWidget(tableEmployes);

    btnAjouter = new QPushButton("➕ Ajouter", left);
    btnModifier = new QPushButton("✏️ Modifier", left);
    btnExtractionAttestation = new QPushButton("📄 Extraction d'attestation du travail", left);
    btnSupprimer = new QPushButton("🗑️ Supprimer", left);

    leftLayout->addWidget(btnAjouter);
    leftLayout->addWidget(btnModifier);
    leftLayout->addWidget(btnExtractionAttestation);
    leftLayout->addWidget(btnSupprimer);

    // RIGHT SIDE (FORM + CHART)
    QWidget *right = new QWidget(pageEmployes);
    QFormLayout *rightEmpLayout = new QFormLayout(right);

    chartTitle = new QLabel("Pourcentage d'employés selon l'âge", right);
    chartTitle->setObjectName("titleLabel");
    chartTitle->setAlignment(Qt::AlignCenter);

    chartWidget = new PieChartWidget(right);

    formTitle = new QLabel("Ajouter un employé", right);
    formTitle->setObjectName("titleLabel");
    formTitle->setAlignment(Qt::AlignCenter);

    editNom = new QLineEdit(right);
    editPrenom = new QLineEdit(right);
    editPoste = new QLineEdit(right);
    editTelephone = new QLineEdit(right);
    editEmail = new QLineEdit(right);

    spinSalaire = new QSpinBox(right);
    spinSalaire->setMaximum(99999);

    spinHeures = new QSpinBox(right);
    spinHeures->setMaximum(500);

    dateEmbauche = new QDateEdit(right);
    dateEmbauche->setCalendarPopup(true);

    dateNaissance = new QDateEdit(right);
    dateNaissance->setCalendarPopup(true);

    btnEnregistrer = new QPushButton("💾 Enregistrer", right);

    rightEmpLayout->addRow(chartTitle);
    rightEmpLayout->addRow(chartWidget);
    rightEmpLayout->addRow(formTitle);
    rightEmpLayout->addRow("Nom :", editNom);
    rightEmpLayout->addRow("Prénom :", editPrenom);
    rightEmpLayout->addRow("Poste :", editPoste);
    rightEmpLayout->addRow("Téléphone :", editTelephone);
    rightEmpLayout->addRow("Email :", editEmail);
    rightEmpLayout->addRow("Salaire :", spinSalaire);
    rightEmpLayout->addRow("Heures :", spinHeures);
    rightEmpLayout->addRow("Date embauche :", dateEmbauche);
    rightEmpLayout->addRow("Date naissance :", dateNaissance);
    rightEmpLayout->addRow(btnEnregistrer);

    empLayout->addWidget(left, 70);
    empLayout->addWidget(right, 30);

    mainStack->addWidget(pageEmployes);

    connect(btnAjouter, &QPushButton::clicked, this, &MainWindow::showAjouter);
    connect(btnModifier, &QPushButton::clicked, this, &MainWindow::showModifier);
    connect(btnSupprimer, &QPushButton::clicked, this, &MainWindow::supprimer);
    connect(btnEnregistrer, &QPushButton::clicked, this, &MainWindow::enregistrer);
    connect(btnExtractionAttestation, &QPushButton::clicked, this, &MainWindow::extractAttestation);
    connect(tableEmployes, &QTableWidget::itemSelectionChanged, this, &MainWindow::tableSelectionChanged);
    connect(editSearch, &QLineEdit::textChanged, this, &MainWindow::searchByName);
    connect(btnSearchEmp, &QPushButton::clicked, this, &MainWindow::searchByName);
    connect(comboSort, &QComboBox::currentIndexChanged, this, &MainWindow::sortBySalary);

    // ======================================
    // PAGE GESTION FOURNISSEURS
    // ======================================
    pageFournisseurs = new QWidget(this);
    QHBoxLayout *fournisseursMainLayout = new QHBoxLayout(pageFournisseurs);
    fournisseursMainLayout->setContentsMargins(0, 0, 0, 0);
    fournisseursMainLayout->setSpacing(0);

    // Menu latéral gauche
    QFrame *menuFrameFournisseurs = new QFrame(pageFournisseurs);
    menuFrameFournisseurs->setObjectName("menuFrame");
    menuFrameFournisseurs->setFixedWidth(250);
    QVBoxLayout *menuLayoutFournisseurs = new QVBoxLayout(menuFrameFournisseurs);
    menuLayoutFournisseurs->setContentsMargins(15, 30, 15, 20);
    menuLayoutFournisseurs->setSpacing(15);

    QLabel *titleMenuFournisseurs = new QLabel("GESTION FOURNISSEURS", menuFrameFournisseurs);
    titleMenuFournisseurs->setStyleSheet("color:white; font-size:18px; font-weight:bold;");
    titleMenuFournisseurs->setAlignment(Qt::AlignCenter);

    btnListeFournisseurs = new QPushButton("📋 Liste des fournisseurs", menuFrameFournisseurs);
    btnAjouterFournisseur = new QPushButton("➕ Ajouter fournisseur", menuFrameFournisseurs);
    btnRetourMenuFournisseurs = new QPushButton("🔙 Retour au menu", menuFrameFournisseurs);

    QString menuBtnStyle =
        "QPushButton { "
        "background-color: #7a8f4e; color: white; "
        "padding: 12px; border-radius: 6px; "
        "font-size: 13px; text-align: left; "
        "}"
        "QPushButton:hover { background-color: #9bb35d; }";

    btnListeFournisseurs->setStyleSheet(menuBtnStyle);
    btnAjouterFournisseur->setStyleSheet(menuBtnStyle);
    btnRetourMenuFournisseurs->setStyleSheet(menuBtnStyle);

    menuLayoutFournisseurs->addWidget(titleMenuFournisseurs);
    menuLayoutFournisseurs->addSpacing(20);
    menuLayoutFournisseurs->addWidget(btnListeFournisseurs);
    menuLayoutFournisseurs->addWidget(btnAjouterFournisseur);
    menuLayoutFournisseurs->addStretch();
    menuLayoutFournisseurs->addWidget(btnRetourMenuFournisseurs);

    // Stacked widget pour les pages fournisseurs
    stackedWidgetFournisseurs = new QStackedWidget(pageFournisseurs);

    // ======================================
    // PAGE LISTE FOURNISSEURS
    // ======================================
    pageListeFournisseurs = new QWidget(stackedWidgetFournisseurs);
    QVBoxLayout *listeFournisseursLayout = new QVBoxLayout(pageListeFournisseurs);
    listeFournisseursLayout->setContentsMargins(20, 20, 20, 20);
    listeFournisseursLayout->setSpacing(15);

    // Header avec recherche et tri
    QHBoxLayout *headerFournisseursLayout = new QHBoxLayout();
    QLabel *titleListeFournisseurs = new QLabel("📋 LISTE DES FOURNISSEURS");
    titleListeFournisseurs->setObjectName("titleLabel");

    searchFournisseurEdit = new QLineEdit(pageListeFournisseurs);
    searchFournisseurEdit->setPlaceholderText("🔍 Rechercher par nom...");
    searchFournisseurEdit->setFixedWidth(250);

    comboSortFournisseurs = new QComboBox(pageListeFournisseurs);
    comboSortFournisseurs->addItem("Trier par nom (A-Z)");
    comboSortFournisseurs->addItem("Trier par nom (Z-A)");
    comboSortFournisseurs->addItem("Trier par date commande");
    comboSortFournisseurs->setFixedWidth(200);

    btnExportPDF = new QPushButton("📄 Exporter facture", pageListeFournisseurs);

    headerFournisseursLayout->addWidget(titleListeFournisseurs);
    headerFournisseursLayout->addStretch();
    headerFournisseursLayout->addWidget(searchFournisseurEdit);
    headerFournisseursLayout->addWidget(comboSortFournisseurs);
    headerFournisseursLayout->addWidget(btnExportPDF);

    // Statistiques fournisseurs
    QHBoxLayout *statsFournisseursLayout = new QHBoxLayout();

    labelTotalFournisseurs = new QLabel("Total fournisseurs: 0");
    labelTotalCommandes = new QLabel("Total commandes: 0");
    labelCommandesEnCours = new QLabel("En cours: 0");
    labelCommandesLivrees = new QLabel("Livrées: 0");
    labelTauxLivraison = new QLabel("Taux livraison: 0%");

    QString statStyle = "QLabel { padding: 10px; background-color: #e8f5e9; border-radius: 6px; font-weight: bold; }";
    labelTotalFournisseurs->setStyleSheet(statStyle);
    labelTotalCommandes->setStyleSheet(statStyle);
    labelCommandesEnCours->setStyleSheet(statStyle);
    labelCommandesLivrees->setStyleSheet(statStyle);
    labelTauxLivraison->setStyleSheet(statStyle);

    statsFournisseursLayout->addWidget(labelTotalFournisseurs);
    statsFournisseursLayout->addWidget(labelTotalCommandes);
    statsFournisseursLayout->addWidget(labelCommandesEnCours);
    statsFournisseursLayout->addWidget(labelCommandesLivrees);
    statsFournisseursLayout->addWidget(labelTauxLivraison);

    // Graphique
    chartFournisseurs = new PieChartWidget(pageListeFournisseurs);

    // Table fournisseurs
    tableFournisseurs = new QTableWidget(pageListeFournisseurs);
    tableFournisseurs->setColumnCount(12);
    tableFournisseurs->setHorizontalHeaderLabels({
        "ID", "Nom", "Email", "Téléphone", "Produit",
        "Date commande", "Date livraison", "Prix HT",
        "Mode paiement", "Statut", "Quantité", "Prix TTC"
    });
    tableFournisseurs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableFournisseurs->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableFournisseurs->setAlternatingRowColors(true);
    tableFournisseurs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Boutons d'action
    QHBoxLayout *btnFournisseursLayout = new QHBoxLayout();
    btnModifierFournisseur = new QPushButton("✏️ Modifier", pageListeFournisseurs);
    btnSupprimerFournisseur = new QPushButton("🗑️ Supprimer", pageListeFournisseurs);
    btnDetailsFournisseur = new QPushButton("📄 Détails", pageListeFournisseurs);

    btnFournisseursLayout->addWidget(btnModifierFournisseur);
    btnFournisseursLayout->addWidget(btnSupprimerFournisseur);
    btnFournisseursLayout->addWidget(btnDetailsFournisseur);
    btnFournisseursLayout->addStretch();

    // Performance fournisseurs
    QLabel *perfTitle = new QLabel("📊 Performance des fournisseurs");
    perfTitle->setObjectName("titleLabel");

    tablePerformance = new QTableWidget(pageListeFournisseurs);
    tablePerformance->setColumnCount(4);
    tablePerformance->setHorizontalHeaderLabels({"Nom fournisseur", "Commandes totales", "Commandes livrées", "Taux (%)"});
    tablePerformance->setMaximumHeight(150);
    tablePerformance->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    labelMeilleurFournisseur = new QLabel("🏆 Meilleur fournisseur: -");
    labelFournisseurRapide = new QLabel("⚡ Fournisseur le plus rapide: -");

    listeFournisseursLayout->addLayout(headerFournisseursLayout);
    listeFournisseursLayout->addLayout(statsFournisseursLayout);
    listeFournisseursLayout->addWidget(chartFournisseurs);
    listeFournisseursLayout->addWidget(tableFournisseurs);
    listeFournisseursLayout->addLayout(btnFournisseursLayout);
    listeFournisseursLayout->addWidget(perfTitle);
    listeFournisseursLayout->addWidget(tablePerformance);
    listeFournisseursLayout->addWidget(labelMeilleurFournisseur);
    listeFournisseursLayout->addWidget(labelFournisseurRapide);

    stackedWidgetFournisseurs->addWidget(pageListeFournisseurs);

    // ======================================
    // PAGE FORMULAIRE FOURNISSEUR
    // ======================================
    pageFormulaireFournisseurs = new QWidget(stackedWidgetFournisseurs);
    QVBoxLayout *formFournisseursLayout = new QVBoxLayout(pageFormulaireFournisseurs);
    formFournisseursLayout->setContentsMargins(40, 30, 40, 30);
    formFournisseursLayout->setSpacing(15);

    QLabel *titleFormFournisseur = new QLabel("➕ AJOUTER/MODIFIER FOURNISSEUR");
    titleFormFournisseur->setObjectName("titleLabel");
    titleFormFournisseur->setAlignment(Qt::AlignCenter);

    QScrollArea *scrollFournisseur = new QScrollArea(pageFormulaireFournisseurs);
    scrollFournisseur->setWidgetResizable(true);
    scrollFournisseur->setFrameShape(QFrame::NoFrame);

    QWidget *scrollContentFournisseur = new QWidget();
    QFormLayout *formLayoutFournisseur = new QFormLayout(scrollContentFournisseur);
    formLayoutFournisseur->setSpacing(12);
    formLayoutFournisseur->setContentsMargins(20, 20, 20, 20);

    // Informations fournisseur
    QLabel *infoFournisseurLabel = new QLabel("Informations du fournisseur");
    infoFournisseurLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #556b2f; margin-top: 10px;");

    editNomFournisseur = new QLineEdit();
    editEmailFournisseur = new QLineEdit();
    editTelephoneFournisseur = new QLineEdit();
    editProduitFournisseur = new QLineEdit();

    // Informations commande
    QLabel *infoCommandeLabel = new QLabel("Informations de la commande");
    infoCommandeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #556b2f; margin-top: 10px;");

    editIDCommande = new QLineEdit();
    editIDCommande->setReadOnly(true);
    editIDCommande->setPlaceholderText("Généré automatiquement");

    editDateCommande = new QDateEdit();
    editDateCommande->setCalendarPopup(true);
    editDateCommande->setDate(QDate::currentDate());

    editDateLivraison = new QDateEdit();
    editDateLivraison->setCalendarPopup(true);
    editDateLivraison->setDate(QDate::currentDate().addDays(7));

    editPrixHT = new QLineEdit();
    editPrixHT->setPlaceholderText("0.00");

    editTVA = new QLineEdit();
    editTVA->setText("19");
    editTVA->setPlaceholderText("19");

    editRemise = new QLineEdit();
    editRemise->setText("0");
    editRemise->setPlaceholderText("0");

    editPrixTTC = new QLineEdit();
    editPrixTTC->setReadOnly(true);
    editPrixTTC->setPlaceholderText("Calculé automatiquement");

    comboModePaiement = new QComboBox();
    comboModePaiement->addItems({"Espèces", "Virement", "Chèque", "Carte bancaire"});

    editAvance = new QLineEdit();
    editAvance->setPlaceholderText("0.00");

    editResteAPayer = new QLineEdit();
    editResteAPayer->setReadOnly(true);
    editResteAPayer->setPlaceholderText("Calculé automatiquement");

    editPrixApresLivraison = new QLineEdit();
    editPrixApresLivraison->setPlaceholderText("0.00");

    btnEnregistrerFournisseur = new QPushButton("💾 Enregistrer");

    formLayoutFournisseur->addRow(infoFournisseurLabel);
    formLayoutFournisseur->addRow("Nom du fournisseur:", editNomFournisseur);
    formLayoutFournisseur->addRow("Email:", editEmailFournisseur);
    formLayoutFournisseur->addRow("Téléphone:", editTelephoneFournisseur);
    formLayoutFournisseur->addRow("Produit:", editProduitFournisseur);

    formLayoutFournisseur->addRow(infoCommandeLabel);
    formLayoutFournisseur->addRow("ID Commande:", editIDCommande);
    formLayoutFournisseur->addRow("Date commande:", editDateCommande);
    formLayoutFournisseur->addRow("Date livraison:", editDateLivraison);
    formLayoutFournisseur->addRow("Prix HT (DT):", editPrixHT);
    formLayoutFournisseur->addRow("TVA (%):", editTVA);
    formLayoutFournisseur->addRow("Remise (%):", editRemise);
    formLayoutFournisseur->addRow("Prix TTC (DT):", editPrixTTC);
    formLayoutFournisseur->addRow("Mode de paiement:", comboModePaiement);
    formLayoutFournisseur->addRow("Avance (DT):", editAvance);
    formLayoutFournisseur->addRow("Reste à payer (DT):", editResteAPayer);
    formLayoutFournisseur->addRow("Prix après livraison (DT):", editPrixApresLivraison);
    formLayoutFournisseur->addRow(btnEnregistrerFournisseur);

    scrollFournisseur->setWidget(scrollContentFournisseur);

    formFournisseursLayout->addWidget(titleFormFournisseur);
    formFournisseursLayout->addWidget(scrollFournisseur);

    stackedWidgetFournisseurs->addWidget(pageFormulaireFournisseurs);

    // ======================================
    // PAGE DÉTAILS FOURNISSEUR
    // ======================================
    pageDetailsFournisseurs = new QWidget(stackedWidgetFournisseurs);
    QVBoxLayout *detailsFournisseursLayout = new QVBoxLayout(pageDetailsFournisseurs);
    detailsFournisseursLayout->setContentsMargins(40, 30, 40, 30);
    detailsFournisseursLayout->setSpacing(15);

    detailsTitle = new QLabel("📄 DÉTAILS DE LA COMMANDE FOURNISSEUR");
    detailsTitle->setObjectName("titleLabel");
    detailsTitle->setAlignment(Qt::AlignCenter);

    QScrollArea *scrollDetail = new QScrollArea(pageDetailsFournisseurs);
    scrollDetail->setWidgetResizable(true);
    scrollDetail->setFrameShape(QFrame::NoFrame);

    QWidget *scrollDetailContent = new QWidget();
    QFormLayout *detailFormLayout = new QFormLayout(scrollDetailContent);
    detailFormLayout->setSpacing(12);
    detailFormLayout->setContentsMargins(20, 20, 20, 20);

    detailIdCommande = new QLineEdit();
    detailIdCommande->setReadOnly(true);
    detailNom = new QLineEdit();
    detailNom->setReadOnly(true);
    detailEmail = new QLineEdit();
    detailEmail->setReadOnly(true);
    detailTelephone = new QLineEdit();
    detailTelephone->setReadOnly(true);
    detailProduit = new QLineEdit();
    detailProduit->setReadOnly(true);
    detailDateCommande = new QLineEdit();
    detailDateCommande->setReadOnly(true);
    detailDateLivraison = new QLineEdit();
    detailDateLivraison->setReadOnly(true);
    detailPrixHT = new QLineEdit();
    detailPrixHT->setReadOnly(true);
    detailModePaiement = new QLineEdit();
    detailModePaiement->setReadOnly(true);
    detailAvance = new QLineEdit();
    detailAvance->setReadOnly(true);
    detailResteAPayer = new QLineEdit();
    detailResteAPayer->setReadOnly(true);
    detailTVA = new QLineEdit();
    detailTVA->setReadOnly(true);
    detailRemise = new QLineEdit();
    detailRemise->setReadOnly(true);
    detailPrixTTC = new QLineEdit();
    detailPrixTTC->setReadOnly(true);
    detailStatut = new QLineEdit();
    detailStatut->setReadOnly(true);
    detailQte = new QLineEdit();
    detailQte->setReadOnly(true);

    btnRetourDetail = new QPushButton("🔙 Retour à la liste");

    detailFormLayout->addRow("ID Commande:", detailIdCommande);
    detailFormLayout->addRow("Nom du fournisseur:", detailNom);
    detailFormLayout->addRow("Email:", detailEmail);
    detailFormLayout->addRow("Téléphone:", detailTelephone);
    detailFormLayout->addRow("Produit:", detailProduit);
    detailFormLayout->addRow("Date commande:", detailDateCommande);
    detailFormLayout->addRow("Date livraison:", detailDateLivraison);
    detailFormLayout->addRow("Prix HT:", detailPrixHT);
    detailFormLayout->addRow("Mode de paiement:", detailModePaiement);
    detailFormLayout->addRow("Avance:", detailAvance);
    detailFormLayout->addRow("Reste à payer:", detailResteAPayer);
    detailFormLayout->addRow("TVA (%):", detailTVA);
    detailFormLayout->addRow("Remise (%):", detailRemise);
    detailFormLayout->addRow("Prix TTC:", detailPrixTTC);
    detailFormLayout->addRow("Statut:", detailStatut);
    detailFormLayout->addRow("Quantité:", detailQte);
    detailFormLayout->addRow(btnRetourDetail);

    scrollDetail->setWidget(scrollDetailContent);

    detailsFournisseursLayout->addWidget(detailsTitle);
    detailsFournisseursLayout->addWidget(scrollDetail);

    stackedWidgetFournisseurs->addWidget(pageDetailsFournisseurs);

    // Assembler la page fournisseurs
    fournisseursMainLayout->addWidget(menuFrameFournisseurs);
    fournisseursMainLayout->addWidget(stackedWidgetFournisseurs);

    mainStack->addWidget(pageFournisseurs);

    // Connexions fournisseurs
    connect(btnListeFournisseurs, &QPushButton::clicked, this, &MainWindow::on_btnListe_clicked);
    connect(btnAjouterFournisseur, &QPushButton::clicked, this, &MainWindow::on_btnAjouter_clicked);
    connect(btnRetourMenuFournisseurs, &QPushButton::clicked, this, &MainWindow::backToMenu);
    connect(btnEnregistrerFournisseur, &QPushButton::clicked, this, &MainWindow::on_btnEnregistrer_clicked);
    connect(btnModifierFournisseur, &QPushButton::clicked, this, &MainWindow::on_btnModifier_clicked);
    connect(btnSupprimerFournisseur, &QPushButton::clicked, this, &MainWindow::on_btnSupprimer_clicked);
    connect(btnDetailsFournisseur, &QPushButton::clicked, this, &MainWindow::on_btnDetails_clicked);
    connect(btnRetourDetail, &QPushButton::clicked, this, &MainWindow::on_btnRetourDetail_clicked);
    connect(searchFournisseurEdit, &QLineEdit::textChanged, this, &MainWindow::searchFournisseur);
    connect(comboSortFournisseurs, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::sortCommandesParNom);
    connect(btnExportPDF, &QPushButton::clicked, this, &MainWindow::exportFacturePDF);
    connect(editPrixHT, &QLineEdit::textChanged, this, &MainWindow::calculatePrixTTC);
    connect(editTVA, &QLineEdit::textChanged, this, &MainWindow::calculatePrixTTC);
    connect(editRemise, &QLineEdit::textChanged, this, &MainWindow::calculatePrixTTC);
    connect(editAvance, &QLineEdit::textChanged, this, &MainWindow::calculateResteAPayer);

    // ======================================
    // PAGE GESTION CLIENTS
    // ======================================
    pageClients = new QWidget(this);
    QHBoxLayout *clientsMainLayout = new QHBoxLayout(pageClients);
    clientsMainLayout->setContentsMargins(0, 0, 0, 0);
    clientsMainLayout->setSpacing(0);

    // Menu latéral gauche
    QFrame *menuFrameClients = new QFrame(pageClients);
    menuFrameClients->setObjectName("menuFrame");
    menuFrameClients->setFixedWidth(250);
    QVBoxLayout *menuLayoutClients = new QVBoxLayout(menuFrameClients);
    menuLayoutClients->setContentsMargins(15, 30, 15, 20);
    menuLayoutClients->setSpacing(15);

    QLabel *titleMenuClients = new QLabel("GESTION CLIENTS", menuFrameClients);
    titleMenuClients->setStyleSheet("color:white; font-size:18px; font-weight:bold;");
    titleMenuClients->setAlignment(Qt::AlignCenter);

    btnListeClients = new QPushButton("📋 Liste des clients", menuFrameClients);
    btnAjouterClient = new QPushButton("➕ Ajouter client", menuFrameClients);
    btnRetourMenuClients = new QPushButton("🔙 Retour au menu", menuFrameClients);

    btnListeClients->setStyleSheet(menuBtnStyle);
    btnAjouterClient->setStyleSheet(menuBtnStyle);
    btnRetourMenuClients->setStyleSheet(menuBtnStyle);

    menuLayoutClients->addWidget(titleMenuClients);
    menuLayoutClients->addSpacing(20);
    menuLayoutClients->addWidget(btnListeClients);
    menuLayoutClients->addWidget(btnAjouterClient);
    menuLayoutClients->addStretch();
    menuLayoutClients->addWidget(btnRetourMenuClients);

    // Stacked widget pour les pages clients
    stackedWidgetClients = new QStackedWidget(pageClients);

    // ======================================
    // PAGE LISTE CLIENTS
    // ======================================
    pageListeClients = new QWidget(stackedWidgetClients);
    QVBoxLayout *listeClientsLayout = new QVBoxLayout(pageListeClients);
    listeClientsLayout->setContentsMargins(20, 20, 20, 20);
    listeClientsLayout->setSpacing(15);

    // Header avec recherche et tri
    QHBoxLayout *headerClientsLayout = new QHBoxLayout();
    QLabel *titleListeClients = new QLabel("📋 LISTE DES CLIENTS");
    titleListeClients->setObjectName("titleLabel");

    searchClientEdit = new QLineEdit(pageListeClients);
    searchClientEdit->setPlaceholderText("🔍 Rechercher par nom...");
    searchClientEdit->setFixedWidth(250);

    comboSortClients = new QComboBox(pageListeClients);
    comboSortClients->addItem("Trier par nom (A-Z)");
    comboSortClients->addItem("Trier par nom (Z-A)");
    comboSortClients->addItem("Trier par date commande");
    comboSortClients->setFixedWidth(200);

    btnExportPDFClient = new QPushButton("📄 Exporter facture", pageListeClients);

    headerClientsLayout->addWidget(titleListeClients);
    headerClientsLayout->addStretch();
    headerClientsLayout->addWidget(searchClientEdit);
    headerClientsLayout->addWidget(comboSortClients);
    headerClientsLayout->addWidget(btnExportPDFClient);

    // Statistiques clients
    QHBoxLayout *statsClientsLayout = new QHBoxLayout();

    labelTotalClients = new QLabel("Total clients: 0");
    labelTotalCommandesClients = new QLabel("Total commandes: 0");
    labelCommandesEnCoursClients = new QLabel("En cours: 0");
    labelCommandesLivreesClients = new QLabel("Livrées: 0");
    labelTauxLivraisonClients = new QLabel("Taux livraison: 0%");

    labelTotalClients->setStyleSheet(statStyle);
    labelTotalCommandesClients->setStyleSheet(statStyle);
    labelCommandesEnCoursClients->setStyleSheet(statStyle);
    labelCommandesLivreesClients->setStyleSheet(statStyle);
    labelTauxLivraisonClients->setStyleSheet(statStyle);

    statsClientsLayout->addWidget(labelTotalClients);
    statsClientsLayout->addWidget(labelTotalCommandesClients);
    statsClientsLayout->addWidget(labelCommandesEnCoursClients);
    statsClientsLayout->addWidget(labelCommandesLivreesClients);
    statsClientsLayout->addWidget(labelTauxLivraisonClients);

    // Graphique
    chartClients = new PieChartWidget(pageListeClients);

    // Table clients
    tableClients = new QTableWidget(pageListeClients);
    tableClients->setColumnCount(12);
    tableClients->setHorizontalHeaderLabels({
        "ID", "Nom", "Email", "Téléphone", "Produit",
        "Date commande", "Date livraison", "Prix HT",
        "Mode paiement", "Statut", "Quantité", "Prix TTC"
    });
    tableClients->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableClients->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableClients->setAlternatingRowColors(true);
    tableClients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Boutons d'action
    QHBoxLayout *btnClientsLayout = new QHBoxLayout();
    btnModifierClient = new QPushButton("✏️ Modifier", pageListeClients);
    btnSupprimerClient = new QPushButton("🗑️ Supprimer", pageListeClients);
    btnDetailsClient = new QPushButton("📄 Détails", pageListeClients);

    btnClientsLayout->addWidget(btnModifierClient);
    btnClientsLayout->addWidget(btnSupprimerClient);
    btnClientsLayout->addWidget(btnDetailsClient);
    btnClientsLayout->addStretch();

    // Performance clients
    QLabel *perfTitleClients = new QLabel("📊 Performance des clients");
    perfTitleClients->setObjectName("titleLabel");

    tablePerformanceClients = new QTableWidget(pageListeClients);
    tablePerformanceClients->setColumnCount(4);
    tablePerformanceClients->setHorizontalHeaderLabels({"Nom client", "Commandes totales", "Commandes livrées", "Taux (%)"});
    tablePerformanceClients->setMaximumHeight(150);
    tablePerformanceClients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    labelMeilleurClient = new QLabel("🏆 Meilleur client: -");
    labelClientRapide = new QLabel("⚡ Client le plus rapide: -");

    listeClientsLayout->addLayout(headerClientsLayout);
    listeClientsLayout->addLayout(statsClientsLayout);
    listeClientsLayout->addWidget(chartClients);
    listeClientsLayout->addWidget(tableClients);
    listeClientsLayout->addLayout(btnClientsLayout);
    listeClientsLayout->addWidget(perfTitleClients);
    listeClientsLayout->addWidget(tablePerformanceClients);
    listeClientsLayout->addWidget(labelMeilleurClient);
    listeClientsLayout->addWidget(labelClientRapide);

    stackedWidgetClients->addWidget(pageListeClients);

    // ======================================
    // PAGE FORMULAIRE CLIENT
    // ======================================
    pageFormulaireClients = new QWidget(stackedWidgetClients);
    QVBoxLayout *formClientsLayout = new QVBoxLayout(pageFormulaireClients);
    formClientsLayout->setContentsMargins(40, 30, 40, 30);
    formClientsLayout->setSpacing(15);

    QLabel *titleFormClient = new QLabel("➕ AJOUTER/MODIFIER CLIENT");
    titleFormClient->setObjectName("titleLabel");
    titleFormClient->setAlignment(Qt::AlignCenter);

    QScrollArea *scrollClient = new QScrollArea(pageFormulaireClients);
    scrollClient->setWidgetResizable(true);
    scrollClient->setFrameShape(QFrame::NoFrame);

    QWidget *scrollContentClient = new QWidget();
    QFormLayout *formLayoutClient = new QFormLayout(scrollContentClient);
    formLayoutClient->setSpacing(12);
    formLayoutClient->setContentsMargins(20, 20, 20, 20);

    // Informations client
    QLabel *infoClientLabel = new QLabel("Informations du client");
    infoClientLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #556b2f; margin-top: 10px;");

    editNomClient = new QLineEdit();
    editEmailClient = new QLineEdit();
    editTelephoneClient = new QLineEdit();
    editProduitClient = new QLineEdit();

    // Informations commande
    QLabel *infoCommandeClientLabel = new QLabel("Informations de la commande");
    infoCommandeClientLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #556b2f; margin-top: 10px;");

    editIDCommandeClient = new QLineEdit();
    editIDCommandeClient->setReadOnly(true);
    editIDCommandeClient->setPlaceholderText("Généré automatiquement");

    editDateCommandeClient = new QDateEdit();
    editDateCommandeClient->setCalendarPopup(true);
    editDateCommandeClient->setDate(QDate::currentDate());

    editDateLivraisonClient = new QDateEdit();
    editDateLivraisonClient->setCalendarPopup(true);
    editDateLivraisonClient->setDate(QDate::currentDate().addDays(7));

    editPrixHTClient = new QLineEdit();
    editPrixHTClient->setPlaceholderText("0.00");

    editTVAClient = new QLineEdit();
    editTVAClient->setText("19");
    editTVAClient->setPlaceholderText("19");

    editRemiseClient = new QLineEdit();
    editRemiseClient->setText("0");
    editRemiseClient->setPlaceholderText("0");

    editPrixTTCClient = new QLineEdit();
    editPrixTTCClient->setReadOnly(true);
    editPrixTTCClient->setPlaceholderText("Calculé automatiquement");

    comboModePaiementClient = new QComboBox();
    comboModePaiementClient->addItems({"Espèces", "Virement", "Chèque", "Carte bancaire"});

    editAvanceClient = new QLineEdit();
    editAvanceClient->setPlaceholderText("0.00");

    editResteAPayerClient = new QLineEdit();
    editResteAPayerClient->setReadOnly(true);
    editResteAPayerClient->setPlaceholderText("Calculé automatiquement");

    editPrixApresLivraisonClient = new QLineEdit();
    editPrixApresLivraisonClient->setPlaceholderText("0.00");

    btnEnregistrerClient = new QPushButton("💾 Enregistrer");

    formLayoutClient->addRow(infoClientLabel);
    formLayoutClient->addRow("Nom du client:", editNomClient);
    formLayoutClient->addRow("Email:", editEmailClient);
    formLayoutClient->addRow("Téléphone:", editTelephoneClient);
    formLayoutClient->addRow("Produit:", editProduitClient);

    formLayoutClient->addRow(infoCommandeClientLabel);
    formLayoutClient->addRow("ID Commande:", editIDCommandeClient);
    formLayoutClient->addRow("Date commande:", editDateCommandeClient);
    formLayoutClient->addRow("Date livraison:", editDateLivraisonClient);
    formLayoutClient->addRow("Prix HT (DT):", editPrixHTClient);
    formLayoutClient->addRow("TVA (%):", editTVAClient);
    formLayoutClient->addRow("Remise (%):", editRemiseClient);
    formLayoutClient->addRow("Prix TTC (DT):", editPrixTTCClient);
    formLayoutClient->addRow("Mode de paiement:", comboModePaiementClient);
    formLayoutClient->addRow("Avance (DT):", editAvanceClient);
    formLayoutClient->addRow("Reste à payer (DT):", editResteAPayerClient);
    formLayoutClient->addRow("Prix après livraison (DT):", editPrixApresLivraisonClient);
    formLayoutClient->addRow(btnEnregistrerClient);

    scrollClient->setWidget(scrollContentClient);

    formClientsLayout->addWidget(titleFormClient);
    formClientsLayout->addWidget(scrollClient);

    stackedWidgetClients->addWidget(pageFormulaireClients);

    // ======================================
    // PAGE DÉTAILS CLIENT
    // ======================================
    pageDetailsClients = new QWidget(stackedWidgetClients);
    QVBoxLayout *detailsClientsLayout = new QVBoxLayout(pageDetailsClients);
    detailsClientsLayout->setContentsMargins(40, 30, 40, 30);
    detailsClientsLayout->setSpacing(15);

    detailsTitleClient = new QLabel("📄 DÉTAILS DE LA COMMANDE CLIENT");
    detailsTitleClient->setObjectName("titleLabel");
    detailsTitleClient->setAlignment(Qt::AlignCenter);

    QScrollArea *scrollDetailClient = new QScrollArea(pageDetailsClients);
    scrollDetailClient->setWidgetResizable(true);
    scrollDetailClient->setFrameShape(QFrame::NoFrame);

    QWidget *scrollDetailContentClient = new QWidget();
    QFormLayout *detailFormLayoutClient = new QFormLayout(scrollDetailContentClient);
    detailFormLayoutClient->setSpacing(12);
    detailFormLayoutClient->setContentsMargins(20, 20, 20, 20);

    detailIdCommandeClient = new QLineEdit();
    detailIdCommandeClient->setReadOnly(true);
    detailNomClient = new QLineEdit();
    detailNomClient->setReadOnly(true);
    detailEmailClient = new QLineEdit();
    detailEmailClient->setReadOnly(true);
    detailTelephoneClient = new QLineEdit();
    detailTelephoneClient->setReadOnly(true);
    detailProduitClient = new QLineEdit();
    detailProduitClient->setReadOnly(true);
    detailDateCommandeClient = new QLineEdit();
    detailDateCommandeClient->setReadOnly(true);
    detailDateLivraisonClient = new QLineEdit();
    detailDateLivraisonClient->setReadOnly(true);
    detailPrixHTClient = new QLineEdit();
    detailPrixHTClient->setReadOnly(true);
    detailModePaiementClient = new QLineEdit();
    detailModePaiementClient->setReadOnly(true);
    detailAvanceClient = new QLineEdit();
    detailAvanceClient->setReadOnly(true);
    detailResteAPayerClient = new QLineEdit();
    detailResteAPayerClient->setReadOnly(true);
    detailTVAClient = new QLineEdit();
    detailTVAClient->setReadOnly(true);
    detailRemiseClient = new QLineEdit();
    detailRemiseClient->setReadOnly(true);
    detailPrixTTCClient = new QLineEdit();
    detailPrixTTCClient->setReadOnly(true);
    detailStatutClient = new QLineEdit();
    detailStatutClient->setReadOnly(true);
    detailQteClient = new QLineEdit();
    detailQteClient->setReadOnly(true);

    btnRetourDetailClient = new QPushButton("🔙 Retour à la liste");

    detailFormLayoutClient->addRow("ID Commande:", detailIdCommandeClient);
    detailFormLayoutClient->addRow("Nom du client:", detailNomClient);
    detailFormLayoutClient->addRow("Email:", detailEmailClient);
    detailFormLayoutClient->addRow("Téléphone:", detailTelephoneClient);
    detailFormLayoutClient->addRow("Produit:", detailProduitClient);
    detailFormLayoutClient->addRow("Date commande:", detailDateCommandeClient);
    detailFormLayoutClient->addRow("Date livraison:", detailDateLivraisonClient);
    detailFormLayoutClient->addRow("Prix HT:", detailPrixHTClient);
    detailFormLayoutClient->addRow("Mode de paiement:", detailModePaiementClient);
    detailFormLayoutClient->addRow("Avance:", detailAvanceClient);
    detailFormLayoutClient->addRow("Reste à payer:", detailResteAPayerClient);
    detailFormLayoutClient->addRow("TVA (%):", detailTVAClient);
    detailFormLayoutClient->addRow("Remise (%):", detailRemiseClient);
    detailFormLayoutClient->addRow("Prix TTC:", detailPrixTTCClient);
    detailFormLayoutClient->addRow("Statut:", detailStatutClient);
    detailFormLayoutClient->addRow("Quantité:", detailQteClient);
    detailFormLayoutClient->addRow(btnRetourDetailClient);

    scrollDetailClient->setWidget(scrollDetailContentClient);

    detailsClientsLayout->addWidget(detailsTitleClient);
    detailsClientsLayout->addWidget(scrollDetailClient);

    stackedWidgetClients->addWidget(pageDetailsClients);

    // Assembler la page clients
    clientsMainLayout->addWidget(menuFrameClients);
    clientsMainLayout->addWidget(stackedWidgetClients);

    mainStack->addWidget(pageClients);

    // Connexions clients
    connect(btnListeClients, &QPushButton::clicked, this, &MainWindow::on_btnListeClients_clicked);
    connect(btnAjouterClient, &QPushButton::clicked, this, &MainWindow::on_btnAjouterClient_clicked);
    connect(btnRetourMenuClients, &QPushButton::clicked, this, &MainWindow::backToMenu);
    connect(btnEnregistrerClient, &QPushButton::clicked, this, &MainWindow::on_btnEnregistrerClient_clicked);
    connect(btnModifierClient, &QPushButton::clicked, this, &MainWindow::on_btnModifierClient_clicked);
    connect(btnSupprimerClient, &QPushButton::clicked, this, &MainWindow::on_btnSupprimerClient_clicked);
    connect(btnDetailsClient, &QPushButton::clicked, this, &MainWindow::on_btnDetailsClient_clicked);
    connect(btnRetourDetailClient, &QPushButton::clicked, this, &MainWindow::on_btnRetourDetailClient_clicked);
    connect(searchClientEdit, &QLineEdit::textChanged, this, &MainWindow::searchClient);
    connect(comboSortClients, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::sortCommandesClients);
    connect(btnExportPDFClient, &QPushButton::clicked, this, &MainWindow::exportFactureClientPDF);
    connect(editPrixHTClient, &QLineEdit::textChanged, this, &MainWindow::calculatePrixTTCClient);
    connect(editTVAClient, &QLineEdit::textChanged, this, &MainWindow::calculatePrixTTCClient);
    connect(editRemiseClient, &QLineEdit::textChanged, this, &MainWindow::calculatePrixTTCClient);
    connect(editAvanceClient, &QLineEdit::textChanged, this, &MainWindow::calculateResteAPayerClient);

    // ======================================
    // PAGE GESTION DE STOCK (PRODUCTION)
    // ======================================
    pageStocks = new QWidget(this);
    pageStocks->setStyleSheet(
        "QMainWindow { background-color: #2d5016; }"
        "QPushButton { background-color: #5c7a3a; color: white; padding: 12px; border-radius: 4px; font-size: 14px; font-weight: bold; text-align: left; }"
        "QPushButton:hover { background-color: #6b8a4a; }"
        "QPushButton:pressed { background-color: #4a6a2a; }"
        "QPushButton#btnModifierStock, QPushButton#btnSupprimerStock, QPushButton#btnDetailsStock { background-color: #5c7a3a; color: white; padding: 10px 20px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 100px; border: 1px solid #4a6a2a; }"
        "QPushButton#btnModifierStock:hover, QPushButton#btnSupprimerStock:hover, QPushButton#btnDetailsStock:hover { background-color: #6b8a4a; }"
        "QLabel { color: #1a3009; font-weight: bold; font-size: 12px; }"
        "QLineEdit, QDateEdit, QComboBox, QSpinBox, QDoubleSpinBox { background-color: white; border: 1px solid #5f6f3e; border-radius: 4px; padding: 5px; font-size: 12px; }"
        "QTableWidget { background-color: white; gridline-color: #d0d0d0; border: 1px solid #5f6f3e; gridline-width: 0px; }"
        "QHeaderView::section { background-color: #87CEEB; color: #1a3009; padding: 6px; font-weight: bold; font-size: 10px; border: none; }"
    );

    QHBoxLayout *stocksMainLayout = new QHBoxLayout(pageStocks);
    stocksMainLayout->setContentsMargins(0, 0, 0, 0);
    stocksMainLayout->setSpacing(0);

    // Main splitter (horizontal: liste à gauche, stats+formulaire à droite)
    mainSplitterStock = new QSplitter(Qt::Horizontal, pageStocks);
    mainSplitterStock->setStyleSheet(
        "QSplitter::handle { background-color: transparent; width: 0px; }"
        "QSplitter::handle:horizontal { width: 0px; }"
    );

    // Section liste (gauche)
    sectionListeStock = new QWidget(mainSplitterStock);
    sectionListeStock->setStyleSheet(
        "background-color: #f5f5dc;"
        "QPushButton#btnModifierStock:hover, QPushButton#btnSupprimerStock:hover, QPushButton#btnDetailsStock:hover { background-color: #6b8a4a; }"
    );

    QVBoxLayout *listeStockLayout = new QVBoxLayout(sectionListeStock);
    listeStockLayout->setContentsMargins(20, 20, 20, 20);
    listeStockLayout->setSpacing(15);

    QLabel *titleLabelStock = new QLabel("Liste des productions", sectionListeStock);
    titleLabelStock->setStyleSheet("font-size: 20px; font-weight: bold; color: #4a6a2a;");

    // Recherche et tri
    QHBoxLayout *searchLayoutStock = new QHBoxLayout();
    QLabel *labelRechercheStock = new QLabel("Rechercher par type :", sectionListeStock);
    comboRechercheTypeStock = new QComboBox(sectionListeStock);
    comboRechercheTypeStock->addItem("Tous");
    comboRechercheTypeStock->addItem("Huile d'olive");
    comboRechercheTypeStock->addItem("Huile végétale");
    comboRechercheTypeStock->addItem("Olive");

    QLabel *labelTriStock = new QLabel("Trier par :", sectionListeStock);
    comboTriStock = new QComboBox(sectionListeStock);
    comboTriStock->addItem("Date (croissant)");
    comboTriStock->addItem("Date (décroissant)");
    comboTriStock->addItem("Type produit");

    searchLayoutStock->addWidget(labelRechercheStock);
    searchLayoutStock->addWidget(comboRechercheTypeStock);
    searchLayoutStock->addStretch();
    searchLayoutStock->addWidget(labelTriStock);
    searchLayoutStock->addWidget(comboTriStock);

    // Table
    tableProductions = new QTableWidget(sectionListeStock);
    tableProductions->setColumnCount(9);
    tableProductions->setHorizontalHeaderLabels({
        "ID", "Identifiant", "Date production", "Type produit",
        "Qte matière (KG)", "Qte produite (L)", "Rendement (%)", "Lot", "Qualité"
    });
    tableProductions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableProductions->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableProductions->setAlternatingRowColors(true);
    tableProductions->horizontalHeader()->setVisible(true);
    tableProductions->verticalHeader()->setVisible(false);
    
    // Set column widths to ensure text is fully visible
    tableProductions->setColumnWidth(0, 50);   // ID
    tableProductions->setColumnWidth(1, 120);  // Identifiant
    tableProductions->setColumnWidth(2, 130);  // Date production
    tableProductions->setColumnWidth(3, 130);  // Type produit
    tableProductions->setColumnWidth(4, 140);  // Qte matière (KG)
    tableProductions->setColumnWidth(5, 140);  // Qte produite (L)
    tableProductions->setColumnWidth(6, 120);  // Rendement (%)
    tableProductions->setColumnWidth(7, 100);  // Lot
    tableProductions->setColumnWidth(8, 150);  // Qualité
    
    // Enable horizontal scrolling if needed
    tableProductions->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableProductions->setRowCount(0);

    // Boutons d'action
    QHBoxLayout *btnStockLayout = new QHBoxLayout();
    btnModifierStock = new QPushButton("✏️ Modifier", sectionListeStock);
    btnModifierStock->setObjectName("btnModifierStock");
    btnModifierStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 10px 20px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 100px; border: 1px solid #4a6a2a;");
    
    btnSupprimerStock = new QPushButton("🗑️ Supprimer", sectionListeStock);
    btnSupprimerStock->setObjectName("btnSupprimerStock");
    btnSupprimerStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 10px 20px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 100px; border: 1px solid #4a6a2a;");
    
    btnDetailsStock = new QPushButton("📄 Détails", sectionListeStock);
    btnDetailsStock->setObjectName("btnDetailsStock");
    btnDetailsStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 10px 20px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 100px; border: 1px solid #4a6a2a;");
    
    btnExportPDFStock = new QPushButton("📄 Exporter PDF", sectionListeStock);
    btnExportPDFStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 10px 20px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 150px; border: 1px solid #4a6a2a;");

    btnStockLayout->addWidget(btnModifierStock);
    btnStockLayout->addWidget(btnSupprimerStock);
    btnStockLayout->addWidget(btnDetailsStock);
    btnStockLayout->addWidget(btnExportPDFStock);

    QHBoxLayout *btnAjouterLayoutStock = new QHBoxLayout();
    btnAjouterListeStock = new QPushButton("➕ Ajouter", sectionListeStock);
    btnAjouterListeStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 10px 20px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 100px; border: 1px solid #4a6a2a;");
    btnAjouterLayoutStock->addWidget(btnAjouterListeStock);
    btnAjouterLayoutStock->addStretch();

    listeStockLayout->addWidget(titleLabelStock);
    listeStockLayout->addLayout(searchLayoutStock);
    listeStockLayout->addWidget(tableProductions);
    listeStockLayout->addLayout(btnStockLayout);
    listeStockLayout->addLayout(btnAjouterLayoutStock);

    // Right splitter (vertical: stats en haut, formulaire en bas)
    rightSplitterStock = new QSplitter(Qt::Vertical, mainSplitterStock);
    rightSplitterStock->setStyleSheet(
        "QSplitter::handle { background-color: transparent; height: 0px; }"
        "QSplitter::handle:vertical { height: 0px; }"
    );

    // Section statistiques
    sectionStatistiquesStock = new QWidget(rightSplitterStock);
    sectionStatistiquesStock->setStyleSheet("background-color: #f5f5dc;");

    QVBoxLayout *statsLayoutStock = new QVBoxLayout(sectionStatistiquesStock);
    statsLayoutStock->setContentsMargins(15, 15, 15, 15);
    statsLayoutStock->setSpacing(10);

    QLabel *titleLabelStatsStock = new QLabel("📊 Statistiques de production", sectionStatistiquesStock);
    titleLabelStatsStock->setStyleSheet("font-size: 16px; font-weight: bold; color: #4a6a2a;");

    pieChartContainerStock = new QWidget(sectionStatistiquesStock);
    pieChartContainerStock->setMinimumHeight(200);
    pieChartWidgetStock = new PieChartWidget(pieChartContainerStock);
    QVBoxLayout *chartLayoutStock = new QVBoxLayout(pieChartContainerStock);
    chartLayoutStock->setContentsMargins(0, 0, 0, 0);
    chartLayoutStock->addWidget(pieChartWidgetStock);

    statsLayoutStock->addWidget(titleLabelStatsStock);
    statsLayoutStock->addWidget(pieChartContainerStock);

    // Section formulaire
    sectionFormulaireStock = new QWidget(rightSplitterStock);
    sectionFormulaireStock->setStyleSheet("background-color: #f5f5dc;");

    QVBoxLayout *formLayoutStock = new QVBoxLayout(sectionFormulaireStock);
    formLayoutStock->setContentsMargins(30, 30, 30, 30);
    formLayoutStock->setSpacing(15);

    QLabel *titleLabelFormStock = new QLabel("➕ Ajouter/Modifier Production", sectionFormulaireStock);
    titleLabelFormStock->setStyleSheet("font-size: 18px; font-weight: bold; color: #4a6a2a;");

    QFormLayout *fieldsLayoutStock = new QFormLayout();
    fieldsLayoutStock->setHorizontalSpacing(10);
    fieldsLayoutStock->setVerticalSpacing(12);

    QLabel *labelIdentifiantStock = new QLabel("Identifiant de production :", sectionFormulaireStock);
    editIdentifiantStock = new QLineEdit(sectionFormulaireStock);
    fieldsLayoutStock->addRow(labelIdentifiantStock, editIdentifiantStock);

    QLabel *labelDateProductionStock = new QLabel("Date de production :", sectionFormulaireStock);
    editDateProductionStock = new QDateEdit(sectionFormulaireStock);
    editDateProductionStock->setCalendarPopup(true);
    editDateProductionStock->setDate(QDate::currentDate());
    editDateProductionStock->setMaximumDate(QDate::currentDate());
    fieldsLayoutStock->addRow(labelDateProductionStock, editDateProductionStock);

    QLabel *labelTypeProduitStock = new QLabel("Type de produit :", sectionFormulaireStock);
    comboTypeProduitStock = new QComboBox(sectionFormulaireStock);
    comboTypeProduitStock->addItem("Huile d'olive");
    comboTypeProduitStock->addItem("Huile végétale");
    comboTypeProduitStock->addItem("Olive");
    fieldsLayoutStock->addRow(labelTypeProduitStock, comboTypeProduitStock);

    QLabel *labelQuantiteMatiereStock = new QLabel("Quantité de matière première (KG):", sectionFormulaireStock);
    editQuantiteMatiereStock = new QLineEdit(sectionFormulaireStock);
    QRegularExpressionValidator *kgValidatorStock = new QRegularExpressionValidator(QRegularExpression(R"(\d+\.?\d*)"), this);
    editQuantiteMatiereStock->setValidator(kgValidatorStock);
    fieldsLayoutStock->addRow(labelQuantiteMatiereStock, editQuantiteMatiereStock);

    labelQuantiteProduiteStock = new QLabel("Quantité produite (LITRES):", sectionFormulaireStock);
    editQuantiteProduiteStock = new QLineEdit(sectionFormulaireStock);
    QRegularExpressionValidator *litreValidatorStock = new QRegularExpressionValidator(QRegularExpression(R"(\d+\.?\d*)"), this);
    editQuantiteProduiteStock->setValidator(litreValidatorStock);
    fieldsLayoutStock->addRow(labelQuantiteProduiteStock, editQuantiteProduiteStock);

    QLabel *labelRendementStock = new QLabel("Rendement (%):", sectionFormulaireStock);
    editRendementStock = new QLineEdit(sectionFormulaireStock);
    editRendementStock->setReadOnly(true);
    fieldsLayoutStock->addRow(labelRendementStock, editRendementStock);

    QLabel *labelLotStock = new QLabel("Lot de production :", sectionFormulaireStock);
    editLotProductionStock = new QLineEdit(sectionFormulaireStock);
    fieldsLayoutStock->addRow(labelLotStock, editLotProductionStock);

    QLabel *labelQualiteStock = new QLabel("Qualité du produit :", sectionFormulaireStock);
    comboQualiteStock = new QComboBox(sectionFormulaireStock);
    comboQualiteStock->addItem("Huile vierge");
    comboQualiteStock->addItem("Huile extra vierge");
    fieldsLayoutStock->addRow(labelQualiteStock, comboQualiteStock);

    QLabel *labelDateExpirationStock = new QLabel("Date d'expiration :", sectionFormulaireStock);
    editDateExpirationStock = new QDateEdit(sectionFormulaireStock);
    editDateExpirationStock->setCalendarPopup(true);
    editDateExpirationStock->setDate(QDate::currentDate().addYears(2));
    fieldsLayoutStock->addRow(labelDateExpirationStock, editDateExpirationStock);

    // Boutons en bas du formulaire
    QHBoxLayout *btnFormLayoutStock = new QHBoxLayout();
    
    // Spacer pour aligner les boutons à droite
    QSpacerItem *horizontalSpacerStock = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    btnFormLayoutStock->addItem(horizontalSpacerStock);
    
    // Bouton Calculer rendement
    btnCalculerRendementStock = new QPushButton("Calculer rendement", sectionFormulaireStock);
    btnCalculerRendementStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 12px 24px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 220px; min-height: 40px; border: 1px solid #4a6a2a;");
    btnFormLayoutStock->addWidget(btnCalculerRendementStock);
    
    // Bouton Enregistrer
    btnEnregistrerStock = new QPushButton("💾 Enregistrer", sectionFormulaireStock);
    btnEnregistrerStock->setStyleSheet("background-color: #5c7a3a; color: white; padding: 12px 24px; border-radius: 4px; font-size: 14px; font-weight: bold; min-width: 180px; min-height: 40px; border: 1px solid #4a6a2a;");
    btnFormLayoutStock->addWidget(btnEnregistrerStock);

    fieldsLayoutStock->addRow("", btnFormLayoutStock);

    formLayoutStock->addWidget(titleLabelFormStock);
    formLayoutStock->addLayout(fieldsLayoutStock);
    formLayoutStock->addStretch();

    // Assembler les splitters
    rightSplitterStock->addWidget(sectionStatistiquesStock);
    rightSplitterStock->addWidget(sectionFormulaireStock);
    mainSplitterStock->addWidget(sectionListeStock);
    mainSplitterStock->addWidget(rightSplitterStock);

    // Set splitter sizes
    QList<int> mainSizesStock;
    mainSizesStock << 600 << 400;
    mainSplitterStock->setSizes(mainSizesStock);

    QList<int> rightSizesStock;
    rightSizesStock << 200 << 400;
    rightSplitterStock->setSizes(rightSizesStock);

    stocksMainLayout->addWidget(mainSplitterStock);

    mainStack->addWidget(pageStocks);

    // Connexions Stock
    connect(btnModifierStock, &QPushButton::clicked, this, &MainWindow::on_btnModifierStock_clicked);
    connect(btnSupprimerStock, &QPushButton::clicked, this, &MainWindow::on_btnSupprimerStock_clicked);
    connect(btnDetailsStock, &QPushButton::clicked, this, &MainWindow::on_btnDetailsStock_clicked);
    connect(btnAjouterListeStock, &QPushButton::clicked, this, &MainWindow::on_btnAjouterListeStock_clicked);
    connect(btnEnregistrerStock, &QPushButton::clicked, this, &MainWindow::on_btnEnregistrerStock_clicked);
    connect(btnCalculerRendementStock, &QPushButton::clicked, this, &MainWindow::on_btnCalculerRendementStock_clicked);
    connect(btnExportPDFStock, &QPushButton::clicked, this, &MainWindow::on_btnExportPDFStock_clicked);
    connect(comboRechercheTypeStock, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboRechercheTypeStock_currentIndexChanged);
    connect(comboTriStock, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboTriStock_currentIndexChanged);
    connect(comboTypeProduitStock, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboTypeProduitStock_currentIndexChanged);

    // Initialiser les statistiques
    QTimer::singleShot(0, this, [this]() {
        genererStatistiquesStock();
    });

    // ======================================
    // PAGE QUIZ
    // ======================================
    pageQuiz = new QWidget(this);
    pageQuiz->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #556b2f, "
        "stop:0.25 #6b8e23, "
        "stop:0.5 #808000, "
        "stop:0.75 #6b8e23, "
        "stop:1 #556b2f); "
        "}"
        );

    QVBoxLayout *quizLayout = new QVBoxLayout(pageQuiz);
    quizLayout->setContentsMargins(60, 40, 60, 40);
    quizLayout->setSpacing(20);

    QWidget *quizContainer = new QWidget(pageQuiz);
    quizContainer->setStyleSheet(
        "QWidget { "
        "background-color: #ffffff; "
        "border-radius: 12px; "
        "}"
        );
    quizContainer->setMaximumWidth(900);

    QVBoxLayout *containerQuizLayout = new QVBoxLayout(quizContainer);
    containerQuizLayout->setContentsMargins(50, 40, 50, 40);
    containerQuizLayout->setSpacing(20);

    quizTitle = new QLabel("🎓 Quiz pédagogique OLIVERAQ");
    quizTitle->setAlignment(Qt::AlignCenter);
    quizTitle->setStyleSheet(
        "QLabel { "
        "font-size: 26px; "
        "font-weight: 600; "
        "color: #556b2f; "
        "margin-bottom: 10px; "
        "}"
        );

    quizIntro = new QLabel(
        "Testez vos connaissances sur OLIVERAQ et découvrez "
        "les fonctionnalités de cette application de gestion !"
        );
    quizIntro->setAlignment(Qt::AlignCenter);
    quizIntro->setWordWrap(true);
    quizIntro->setStyleSheet(
        "QLabel { "
        "font-size: 14px; "
        "color: #6b8e23; "
        "margin-bottom: 20px; "
        "}"
        );

    quizProgressLabel = new QLabel("Question 1 / 5");
    quizProgressLabel->setAlignment(Qt::AlignCenter);
    quizProgressLabel->setStyleSheet(
        "QLabel { "
        "font-size: 13px; "
        "font-weight: 500; "
        "color: #808000; "
        "margin-bottom: 15px; "
        "}"
        );

    quizQuestionLabel = new QLabel();
    quizQuestionLabel->setAlignment(Qt::AlignLeft);
    quizQuestionLabel->setWordWrap(true);
    quizQuestionLabel->setStyleSheet(
        "QLabel { "
        "font-size: 16px; "
        // =======================
        // CONTINUATION OF MAINWINDOW.CPP
        // Add this after the Quiz page setup in setupUI()
        // =======================

        "font-weight: 500; "
        "color: #2c3e50; "
        "padding: 15px; "
        "background-color: #f9f9f9; "
        "border-radius: 8px; "
        "margin-bottom: 20px; "
        "}"
        );

    quizOption1 = new QRadioButton();
    quizOption2 = new QRadioButton();
    quizOption3 = new QRadioButton();

    QString radioStyle =
        "QRadioButton { "
        "font-size: 14px; "
        "color: #2c3e50; "
        "padding: 10px; "
        "}"
        "QRadioButton::indicator { "
        "width: 18px; "
        "height: 18px; "
        "}"
        "QRadioButton::indicator:checked { "
        "background-color: #6b8e23; "
        "border: 2px solid #556b2f; "
        "border-radius: 9px; "
        "}";

    quizOption1->setStyleSheet(radioStyle);
    quizOption2->setStyleSheet(radioStyle);
    quizOption3->setStyleSheet(radioStyle);

    quizFeedbackLabel = new QLabel();
    quizFeedbackLabel->setAlignment(Qt::AlignCenter);
    quizFeedbackLabel->setWordWrap(true);
    quizFeedbackLabel->setStyleSheet(
        "QLabel { "
        "font-size: 13px; "
        "padding: 12px; "
        "border-radius: 6px; "
        "margin-top: 10px; "
        "}"
        );
    quizFeedbackLabel->hide();

    QHBoxLayout *btnQuizLayout = new QHBoxLayout();
    btnQuizNext = new QPushButton("Suivant");
    btnQuizRestart = new QPushButton("Recommencer");
    btnQuizBackToMenu = new QPushButton("Retour au menu");

    btnQuizNext->setStyleSheet(
        "QPushButton { "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #ffffff; "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #6b8e23, stop:1 #556b2f); "
        "border: none; "
        "border-radius: 6px; "
        "padding: 12px 30px; "
        "min-width: 120px; "
        "}"
        "QPushButton:hover { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #7a9e2d, stop:1 #6b8e23); "
        "}"
        );

    btnQuizRestart->setStyleSheet(btnQuizNext->styleSheet());
    btnQuizBackToMenu->setStyleSheet(
        "QPushButton { "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #556b2f; "
        "background-color: #ffffff; "
        "border: 2px solid #6b8e23; "
        "border-radius: 6px; "
        "padding: 12px 30px; "
        "min-width: 120px; "
        "}"
        "QPushButton:hover { "
        "background-color: #fafff0; "
        "}"
        );

    btnQuizRestart->hide();

    btnQuizLayout->addStretch();
    btnQuizLayout->addWidget(btnQuizBackToMenu);
    btnQuizLayout->addWidget(btnQuizRestart);
    btnQuizLayout->addWidget(btnQuizNext);
    btnQuizLayout->addStretch();

    containerQuizLayout->addWidget(quizTitle);
    containerQuizLayout->addWidget(quizIntro);
    containerQuizLayout->addWidget(quizProgressLabel);
    containerQuizLayout->addWidget(quizQuestionLabel);
    containerQuizLayout->addWidget(quizOption1);
    containerQuizLayout->addWidget(quizOption2);
    containerQuizLayout->addWidget(quizOption3);
    containerQuizLayout->addWidget(quizFeedbackLabel);
    containerQuizLayout->addStretch();
    containerQuizLayout->addLayout(btnQuizLayout);

    quizLayout->addStretch();
    quizLayout->addWidget(quizContainer, 0, Qt::AlignCenter);
    quizLayout->addStretch();

    mainStack->addWidget(pageQuiz);

    connect(btnQuizNext, &QPushButton::clicked, this, &MainWindow::handleQuizNext);
    connect(btnQuizRestart, &QPushButton::clicked, this, &MainWindow::handleQuizRestart);
    connect(btnQuizBackToMenu, &QPushButton::clicked, this, &MainWindow::backToMenu);

    initializeQuiz();
}

// =======================
// IMPLÉMENTATION - NAVIGATION
// =======================

void MainWindow::openGestionEmployes()
{
    mainStack->setCurrentWidget(pageEmployes);
    updateStatistics();
}

void MainWindow::openChangePassword()
{
    mainStack->setCurrentWidget(pageChangePassword);
    editOldPass->clear();
    editNewPass->clear();
    editConfirmPass->clear();
}

void MainWindow::backToMenu()
{
    mainStack->setCurrentWidget(pageMenu);
}

void MainWindow::openGestionFournisseurs()
{
    mainStack->setCurrentWidget(pageFournisseurs);
    stackedWidgetFournisseurs->setCurrentWidget(pageListeFournisseurs);
    updateFournisseurStatistics();
    updatePerformanceMetrics();
}

void MainWindow::openGestionClients()
{
    mainStack->setCurrentWidget(pageClients);
    stackedWidgetClients->setCurrentWidget(pageListeClients);
    updateClientStatistics();
    updateClientPerformance();
}

void MainWindow::openGestionStocks()
{
    mainStack->setCurrentWidget(pageStocks);
    genererStatistiquesStock();
}

void MainWindow::openQuiz()
{
    mainStack->setCurrentWidget(pageQuiz);
    handleQuizRestart();
}

// =======================
// IMPLÉMENTATION - MOT DE PASSE
// =======================

void MainWindow::changePassword()
{
    QString oldPass = editOldPass->text();
    QString newPass = editNewPass->text();
    QString confirmPass = editConfirmPass->text();

    if (oldPass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    if (oldPass != currentPassword) {
        QMessageBox::warning(this, "Erreur", "L'ancien mot de passe est incorrect.");
        return;
    }

    if (newPass != confirmPass) {
        QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
        return;
    }

    if (newPass.length() < 4) {
        QMessageBox::warning(this, "Erreur", "Le mot de passe doit contenir au moins 4 caractères.");
        return;
    }

    currentPassword = newPass;
    QMessageBox::information(this, "Succès", "Mot de passe modifié avec succès !");
    backToMenu();
}

// =======================
// IMPLÉMENTATION - EMPLOYÉS
// =======================

void MainWindow::showAjouter()
{
    selectedRow = -1;
    formTitle->setText("Ajouter un employé");
    editNom->clear();
    editPrenom->clear();
    editPoste->clear();
    editTelephone->clear();
    editEmail->clear();
    spinSalaire->setValue(0);
    spinHeures->setValue(0);
    dateEmbauche->setDate(QDate::currentDate());
    dateNaissance->setDate(QDate::currentDate().addYears(-25));
}

void MainWindow::showModifier()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un employé à modifier.");
        return;
    }

    selectedRow = row;
    formTitle->setText("Modifier un employé");

    editNom->setText(tableEmployes->item(row, 1)->text());
    editPrenom->setText(tableEmployes->item(row, 2)->text());
    editPoste->setText(tableEmployes->item(row, 3)->text());
    editEmail->setText(tableEmployes->item(row, 4)->text());
    editTelephone->setText(tableEmployes->item(row, 5)->text());
    spinSalaire->setValue(tableEmployes->item(row, 6)->text().toInt());
    spinHeures->setValue(tableEmployes->item(row, 7)->text().toInt());
    dateEmbauche->setDate(QDate::fromString(tableEmployes->item(row, 8)->text(), "dd/MM/yyyy"));
    dateNaissance->setDate(QDate::fromString(tableEmployes->item(row, 9)->text(), "dd/MM/yyyy"));
}

void MainWindow::enregistrer()
{
    QString nom = editNom->text().trimmed();
    QString prenom = editPrenom->text().trimmed();
    QString poste = editPoste->text().trimmed();
    QString tel = editTelephone->text().trimmed();
    QString email = editEmail->text().trimmed();
    int salaire = spinSalaire->value();
    int heures = spinHeures->value();
    QString dateEmb = dateEmbauche->date().toString("dd/MM/yyyy");
    QString dateNais = dateNaissance->date().toString("dd/MM/yyyy");

    if (nom.isEmpty() || prenom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir au moins le nom et le prénom.");
        return;
    }

    if (selectedRow == -1) {
        // Ajout
        int row = tableEmployes->rowCount();
        tableEmployes->insertRow(row);

        tableEmployes->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        tableEmployes->setItem(row, 1, new QTableWidgetItem(nom));
        tableEmployes->setItem(row, 2, new QTableWidgetItem(prenom));
        tableEmployes->setItem(row, 3, new QTableWidgetItem(poste));
        tableEmployes->setItem(row, 4, new QTableWidgetItem(email));
        tableEmployes->setItem(row, 5, new QTableWidgetItem(tel));
        tableEmployes->setItem(row, 6, new QTableWidgetItem(QString::number(salaire)));
        tableEmployes->setItem(row, 7, new QTableWidgetItem(QString::number(heures)));
        tableEmployes->setItem(row, 8, new QTableWidgetItem(dateEmb));
        tableEmployes->setItem(row, 9, new QTableWidgetItem(dateNais));

        QMessageBox::information(this, "Succès", "Employé ajouté avec succès !");
    } else {
        // Modification
        tableEmployes->item(selectedRow, 1)->setText(nom);
        tableEmployes->item(selectedRow, 2)->setText(prenom);
        tableEmployes->item(selectedRow, 3)->setText(poste);
        tableEmployes->item(selectedRow, 4)->setText(email);
        tableEmployes->item(selectedRow, 5)->setText(tel);
        tableEmployes->item(selectedRow, 6)->setText(QString::number(salaire));
        tableEmployes->item(selectedRow, 7)->setText(QString::number(heures));
        tableEmployes->item(selectedRow, 8)->setText(dateEmb);
        tableEmployes->item(selectedRow, 9)->setText(dateNais);

        QMessageBox::information(this, "Succès", "Employé modifié avec succès !");
    }

    reindexIds();
    updateStatistics();
    showAjouter();
}

void MainWindow::supprimer()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un employé à supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cet employé ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        tableEmployes->removeRow(row);
        reindexIds();
        updateStatistics();
        QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
    }
}

void MainWindow::tableSelectionChanged()
{
    // Optionnel: peut être utilisé pour des actions lors de la sélection
}

void MainWindow::searchByName()
{
    QString query = editSearch->text().trimmed().toLower();

    for (int i = 0; i < tableEmployes->rowCount(); ++i) {
        QString nom = tableEmployes->item(i, 1)->text().toLower();
        QString prenom = tableEmployes->item(i, 2)->text().toLower();
        bool match = nom.contains(query) || prenom.contains(query);
        tableEmployes->setRowHidden(i, !match);
    }
}

void MainWindow::sortBySalary()
{
    int index = comboSort->currentIndex();

    if (index == 0) {
        // Croissant
        tableEmployes->sortItems(6, Qt::AscendingOrder);
    } else if (index == 1) {
        // Décroissant
        tableEmployes->sortItems(6, Qt::DescendingOrder);
    }
}

void MainWindow::extractAttestation()
{
    int row = tableEmployes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un employé.");
        return;
    }

    QString nom = tableEmployes->item(row, 1)->text();
    QString prenom = tableEmployes->item(row, 2)->text();
    QString poste = tableEmployes->item(row, 3)->text();
    QString dateEmb = tableEmployes->item(row, 8)->text();

    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer attestation",
                                                    "Attestation_" + nom + "_" + prenom + ".pdf",
                                                    "PDF (*.pdf)");

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));

    QString html = "<html><head><style>"
                   "body { font-family: Arial; margin: 60px; }"
                   "h1 { color: #556b2f; text-align: center; margin-bottom: 40px; }"
                   "p { font-size: 14px; line-height: 1.8; }"
                   ".signature { margin-top: 80px; text-align: right; }"
                   "</style></head><body>";

    html += "<h1>ATTESTATION DE TRAVAIL</h1>";
    html += "<p>Je soussigné(e), représentant(e) de la société OLIVERAQ,</p>";
    html += "<p>Certifie que <b>" + prenom + " " + nom + "</b> occupe le poste de <b>" + poste + "</b> "
                                                                                                 "au sein de notre établissement depuis le <b>" + dateEmb + "</b>.</p>";
    html += "<p>Cette attestation est délivrée à l'intéressé(e) pour servir et valoir ce que de droit.</p>";
    html += "<p>Fait à Tunis, le " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>";
    html += "<div class='signature'>";
    html += "<p><b>La Direction</b></p>";
    html += "</div>";
    html += "</body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Succès", "Attestation générée avec succès !");
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void MainWindow::reindexIds()
{
    for (int i = 0; i < tableEmployes->rowCount(); ++i) {
        tableEmployes->item(i, 0)->setText(QString::number(i + 1));
    }
}

void MainWindow::updateStatistics()
{
    int total = tableEmployes->rowCount();
    if (total == 0) {
        chartWidget->setData(QMap<QString, int>(), 0);
        return;
    }

    int jeunes = 0;      // < 30 ans
    int adultes = 0;     // 30-50 ans
    int seniors = 0;     // > 50 ans

    QDate today = QDate::currentDate();

    for (int i = 0; i < total; ++i) {
        QString dateNaisStr = tableEmployes->item(i, 9)->text();
        QDate dateNais = QDate::fromString(dateNaisStr, "dd/MM/yyyy");
        int age = dateNais.daysTo(today) / 365;

        if (age < 30) {
            jeunes++;
        } else if (age <= 50) {
            adultes++;
        } else {
            seniors++;
        }
    }

    QMap<QString, int> data;
    int totalCount = 0;

    if (jeunes > 0) {
        data["< 30 ans"] = jeunes;
        totalCount += jeunes;
    }
    if (adultes > 0) {
        data["30-50 ans"] = adultes;
        totalCount += adultes;
    }
    if (seniors > 0) {
        data["> 50 ans"] = seniors;
        totalCount += seniors;
    }

    chartWidget->setData(data, totalCount);
}

// =======================
// IMPLÉMENTATION - FOURNISSEURS
// =======================

void MainWindow::on_btnListe_clicked()
{
    stackedWidgetFournisseurs->setCurrentWidget(pageListeFournisseurs);
    currentRowFournisseur = -1;
    updateFournisseurStatistics();
    updatePerformanceMetrics();
}

void MainWindow::on_btnAjouter_clicked()
{
    currentRowFournisseur = -1;
    stackedWidgetFournisseurs->setCurrentWidget(pageFormulaireFournisseurs);

    editNomFournisseur->clear();
    editEmailFournisseur->clear();
    editTelephoneFournisseur->clear();
    editProduitFournisseur->clear();
    editIDCommande->clear();
    editDateCommande->setDate(QDate::currentDate());
    editDateLivraison->setDate(QDate::currentDate().addDays(7));
    editPrixHT->clear();
    editTVA->setText("19");
    editRemise->setText("0");
    editPrixTTC->clear();
    comboModePaiement->setCurrentIndex(0);
    editAvance->clear();
    editResteAPayer->clear();
    editPrixApresLivraison->clear();
}

void MainWindow::on_btnEnregistrer_clicked()
{
    QString nom = editNomFournisseur->text().trimmed();
    QString email = editEmailFournisseur->text().trimmed();
    QString tel = editTelephoneFournisseur->text().trimmed();
    QString produit = editProduitFournisseur->text().trimmed();
    QString dateCmd = editDateCommande->date().toString("dd/MM/yyyy");
    QString dateLiv = editDateLivraison->date().toString("dd/MM/yyyy");
    QString prixHT = editPrixHT->text().trimmed();
    QString modePaie = comboModePaiement->currentText();
    QString tva = editTVA->text().trimmed();
    QString remise = editRemise->text().trimmed();
    QString prixTTC = editPrixTTC->text().trimmed();
    QString avance = editAvance->text().trimmed();
    QString reste = editResteAPayer->text().trimmed();

    if (nom.isEmpty() || prixHT.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir au moins le nom du fournisseur et le prix HT.");
        return;
    }

    QDate today = QDate::currentDate();
    QDate livDate = editDateLivraison->date();
    QString statut = (livDate <= today) ? "Livrée" : "En cours";

    if (currentRowFournisseur == -1) {
        // Ajout
        int row = tableFournisseurs->rowCount();
        tableFournisseurs->insertRow(row);

        QString idCmd = "CMD-F-" + QString::number(row + 1);

        tableFournisseurs->setItem(row, 0, new QTableWidgetItem(idCmd));
        tableFournisseurs->setItem(row, 1, new QTableWidgetItem(nom));
        tableFournisseurs->setItem(row, 2, new QTableWidgetItem(email));
        tableFournisseurs->setItem(row, 3, new QTableWidgetItem(tel));
        tableFournisseurs->setItem(row, 4, new QTableWidgetItem(produit));
        tableFournisseurs->setItem(row, 5, new QTableWidgetItem(dateCmd));
        tableFournisseurs->setItem(row, 6, new QTableWidgetItem(dateLiv));
        tableFournisseurs->setItem(row, 7, new QTableWidgetItem(prixHT));
        tableFournisseurs->setItem(row, 8, new QTableWidgetItem(modePaie));
        tableFournisseurs->setItem(row, 9, new QTableWidgetItem(statut));
        tableFournisseurs->setItem(row, 10, new QTableWidgetItem("1")); // Quantité par défaut
        tableFournisseurs->setItem(row, 11, new QTableWidgetItem(prixTTC));

        QMessageBox::information(this, "Succès", "Fournisseur ajouté avec succès !");
    } else {
        // Modification
        tableFournisseurs->item(currentRowFournisseur, 1)->setText(nom);
        tableFournisseurs->item(currentRowFournisseur, 2)->setText(email);
        tableFournisseurs->item(currentRowFournisseur, 3)->setText(tel);
        tableFournisseurs->item(currentRowFournisseur, 4)->setText(produit);
        tableFournisseurs->item(currentRowFournisseur, 5)->setText(dateCmd);
        tableFournisseurs->item(currentRowFournisseur, 6)->setText(dateLiv);
        tableFournisseurs->item(currentRowFournisseur, 7)->setText(prixHT);
        tableFournisseurs->item(currentRowFournisseur, 8)->setText(modePaie);
        tableFournisseurs->item(currentRowFournisseur, 9)->setText(statut);
        tableFournisseurs->item(currentRowFournisseur, 11)->setText(prixTTC);

        QMessageBox::information(this, "Succès", "Fournisseur modifié avec succès !");
    }

    stackedWidgetFournisseurs->setCurrentWidget(pageListeFournisseurs);
    updateFournisseurStatistics();
    updatePerformanceMetrics();
}

void MainWindow::on_btnModifier_clicked()
{
    int row = tableFournisseurs->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un fournisseur à modifier.");
        return;
    }

    currentRowFournisseur = row;
    stackedWidgetFournisseurs->setCurrentWidget(pageFormulaireFournisseurs);

    editIDCommande->setText(tableFournisseurs->item(row, 0)->text());
    editNomFournisseur->setText(tableFournisseurs->item(row, 1)->text());
    editEmailFournisseur->setText(tableFournisseurs->item(row, 2)->text());
    editTelephoneFournisseur->setText(tableFournisseurs->item(row, 3)->text());
    editProduitFournisseur->setText(tableFournisseurs->item(row, 4)->text());
    editDateCommande->setDate(QDate::fromString(tableFournisseurs->item(row, 5)->text(), "dd/MM/yyyy"));
    editDateLivraison->setDate(QDate::fromString(tableFournisseurs->item(row, 6)->text(), "dd/MM/yyyy"));
    editPrixHT->setText(tableFournisseurs->item(row, 7)->text());
    comboModePaiement->setCurrentText(tableFournisseurs->item(row, 8)->text());
}

void MainWindow::on_btnSupprimer_clicked()
{
    int row = tableFournisseurs->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un fournisseur à supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation", "Êtes-vous sûr de vouloir supprimer ce fournisseur ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        tableFournisseurs->removeRow(row);
        QMessageBox::information(this, "Succès", "Fournisseur supprimé avec succès !");
        updateFournisseurStatistics();
        updatePerformanceMetrics();
    }
}

void MainWindow::on_btnDetails_clicked()
{
    int row = tableFournisseurs->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un fournisseur pour voir les détails.");
        return;
    }

    stackedWidgetFournisseurs->setCurrentWidget(pageDetailsFournisseurs);

    detailIdCommande->setText(tableFournisseurs->item(row, 0)->text());
    detailNom->setText(tableFournisseurs->item(row, 1)->text());
    detailEmail->setText(tableFournisseurs->item(row, 2)->text());
    detailTelephone->setText(tableFournisseurs->item(row, 3)->text());
    detailProduit->setText(tableFournisseurs->item(row, 4)->text());
    detailDateCommande->setText(tableFournisseurs->item(row, 5)->text());
    detailDateLivraison->setText(tableFournisseurs->item(row, 6)->text());
    detailPrixHT->setText(tableFournisseurs->item(row, 7)->text() + " DT");
    detailModePaiement->setText(tableFournisseurs->item(row, 8)->text());
    detailStatut->setText(tableFournisseurs->item(row, 9)->text());
    detailQte->setText(tableFournisseurs->item(row, 10)->text());
    detailPrixTTC->setText(tableFournisseurs->item(row, 11)->text() + " DT");

    detailTVA->setText("19%");
    detailRemise->setText("0%");
    detailAvance->setText("0 DT");
    detailResteAPayer->setText(tableFournisseurs->item(row, 11)->text() + " DT");
}

void MainWindow::on_btnRetourDetail_clicked()
{
    stackedWidgetFournisseurs->setCurrentWidget(pageListeFournisseurs);
}

void MainWindow::searchFournisseur()
{
    QString query = searchFournisseurEdit->text().trimmed().toLower();

    for (int i = 0; i < tableFournisseurs->rowCount(); ++i) {
        QString nom = tableFournisseurs->item(i, 1)->text().toLower();
        bool match = nom.contains(query);
        tableFournisseurs->setRowHidden(i, !match);
    }
}

void MainWindow::sortCommandesParNom()
{
    int index = comboSortFournisseurs->currentIndex();

    if (index == 0) {
        // Trier par nom A-Z
        tableFournisseurs->sortItems(1, Qt::AscendingOrder);
    } else if (index == 1) {
        // Trier par nom Z-A
        tableFournisseurs->sortItems(1, Qt::DescendingOrder);
    } else if (index == 2) {
        // Trier par date commande
        tableFournisseurs->sortItems(5, Qt::DescendingOrder);
    }
}

void MainWindow::exportFacturePDF()
{
    int row = tableFournisseurs->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une commande fournisseur.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer facture",
                                                    "Facture_Fournisseur_" + tableFournisseurs->item(row, 1)->text() + ".pdf",
                                                    "PDF (*.pdf)");

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));

    QString html = "<html><head><style>"
                   "body { font-family: Arial; margin: 40px; }"
                   "h1 { color: #556b2f; text-align: center; }"
                   "table { width: 100%; border-collapse: collapse; margin-top: 20px; }"
                   "td, th { border: 1px solid #ddd; padding: 12px; text-align: left; }"
                   "th { background-color: #f0f0f0; font-weight: bold; }"
                   ".total { font-size: 18px; font-weight: bold; color: #556b2f; }"
                   "</style></head><body>";

    html += "<h1>FACTURE FOURNISSEUR</h1>";
    html += "<p><b>Date:</b> " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>";
    html += "<p><b>ID Commande:</b> " + tableFournisseurs->item(row, 0)->text() + "</p>";

    html += "<table>";
    html += "<tr><th>Fournisseur</th><td>" + tableFournisseurs->item(row, 1)->text() + "</td></tr>";
    html += "<tr><th>Email</th><td>" + tableFournisseurs->item(row, 2)->text() + "</td></tr>";
    html += "<tr><th>Téléphone</th><td>" + tableFournisseurs->item(row, 3)->text() + "</td></tr>";
    html += "<tr><th>Produit</th><td>" + tableFournisseurs->item(row, 4)->text() + "</td></tr>";
    html += "<tr><th>Date commande</th><td>" + tableFournisseurs->item(row, 5)->text() + "</td></tr>";
    html += "<tr><th>Date livraison</th><td>" + tableFournisseurs->item(row, 6)->text() + "</td></tr>";
    html += "<tr><th>Prix HT</th><td>" + tableFournisseurs->item(row, 7)->text() + " DT</td></tr>";
    html += "<tr><th>TVA</th><td>19%</td></tr>";
    html += "<tr><th class='total'>Prix TTC</th><td class='total'>" + tableFournisseurs->item(row, 11)->text() + " DT</td></tr>";
    html += "<tr><th>Mode de paiement</th><td>" + tableFournisseurs->item(row, 8)->text() + "</td></tr>";
    html += "<tr><th>Statut</th><td>" + tableFournisseurs->item(row, 9)->text() + "</td></tr>";
    html += "</table>";

    html += "</body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Succès", "Facture exportée avec succès !");
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void MainWindow::updateFournisseurStatistics()
{
    int totalFournisseurs = tableFournisseurs->rowCount();
    int totalCommandes = totalFournisseurs;
    int enCours = 0;
    int livrees = 0;

    for (int i = 0; i < tableFournisseurs->rowCount(); ++i) {
        QString statut = tableFournisseurs->item(i, 9)->text();
        if (statut == "En cours") {
            enCours++;
        } else if (statut == "Livrée") {
            livrees++;
        }
    }

    double tauxLivraison = (totalCommandes > 0) ? (livrees * 100.0 / totalCommandes) : 0.0;

    labelTotalFournisseurs->setText("Total fournisseurs: " + QString::number(totalFournisseurs));
    labelTotalCommandes->setText("Total commandes: " + QString::number(totalCommandes));
    labelCommandesEnCours->setText("En cours: " + QString::number(enCours));
    labelCommandesLivrees->setText("Livrées: " + QString::number(livrees));
    labelTauxLivraison->setText("Taux livraison: " + QString::number(tauxLivraison, 'f', 1) + "%");

    QMap<QString, int> data;
    int totalCount = 0;

    if (enCours > 0) {
        data["En cours"] = enCours;
        totalCount += enCours;
    }
    if (livrees > 0) {
        data["Livrées"] = livrees;
        totalCount += livrees;
    }

    if (!data.isEmpty()) {
        chartFournisseurs->setData(data, totalCount);
    }
}

void MainWindow::updatePerformanceMetrics()
{
    tablePerformance->setRowCount(0);

    QMap<QString, int> commandesTotales;
    QMap<QString, int> commandesLivrees;

    for (int i = 0; i < tableFournisseurs->rowCount(); ++i) {
        QString nom = tableFournisseurs->item(i, 1)->text();
        QString statut = tableFournisseurs->item(i, 9)->text();

        commandesTotales[nom]++;
        if (statut == "Livrée") {
            commandesLivrees[nom]++;
        }
    }

    for (auto it = commandesTotales.begin(); it != commandesTotales.end(); ++it) {
        QString nom = it.key();
        int total = it.value();
        int livrees = commandesLivrees.value(nom, 0);
        double taux = (total > 0) ? (livrees * 100.0 / total) : 0.0;

        int row = tablePerformance->rowCount();
        tablePerformance->insertRow(row);
        tablePerformance->setItem(row, 0, new QTableWidgetItem(nom));
        tablePerformance->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
        tablePerformance->setItem(row, 2, new QTableWidgetItem(QString::number(livrees)));
        tablePerformance->setItem(row, 3, new QTableWidgetItem(QString::number(taux, 'f', 1) + "%"));
    }

    QString meilleur = "-";
    int maxCommandes = 0;
    for (auto it = commandesTotales.begin(); it != commandesTotales.end(); ++it) {
        if (it.value() > maxCommandes) {
            maxCommandes = it.value();
            meilleur = it.key();
        }
    }

    labelMeilleurFournisseur->setText("🏆 Meilleur fournisseur: " + meilleur + " (" + QString::number(maxCommandes) + " commandes)");
    labelFournisseurRapide->setText("⚡ Fournisseur le plus rapide: " + meilleur);
}

void MainWindow::calculatePrixTTC()
{
    double prixHT = editPrixHT->text().toDouble();
    double tva = editTVA->text().toDouble();
    double remise = editRemise->text().toDouble();

    double prixApresRemise = prixHT * (1 - remise / 100.0);
    double prixTTC = prixApresRemise * (1 + tva / 100.0);

    editPrixTTC->setText(QString::number(prixTTC, 'f', 2));
    calculateResteAPayer();
}

void MainWindow::calculateResteAPayer()
{
    double prixTTC = editPrixTTC->text().toDouble();
    double avance = editAvance->text().toDouble();
    double reste = prixTTC - avance;

    editResteAPayer->setText(QString::number(reste, 'f', 2));
}

// =======================
// IMPLÉMENTATION - CLIENTS
// =======================

void MainWindow::on_btnListeClients_clicked()
{
    stackedWidgetClients->setCurrentWidget(pageListeClients);
    currentRowClient = -1;
    updateClientStatistics();
    updateClientPerformance();
}

void MainWindow::on_btnAjouterClient_clicked()
{
    currentRowClient = -1;
    stackedWidgetClients->setCurrentWidget(pageFormulaireClients);

    editNomClient->clear();
    editEmailClient->clear();
    editTelephoneClient->clear();
    editProduitClient->clear();
    editIDCommandeClient->clear();
    editDateCommandeClient->setDate(QDate::currentDate());
    editDateLivraisonClient->setDate(QDate::currentDate().addDays(7));
    editPrixHTClient->clear();
    editTVAClient->setText("19");
    editRemiseClient->setText("0");
    editPrixTTCClient->clear();
    comboModePaiementClient->setCurrentIndex(0);
    editAvanceClient->clear();
    editResteAPayerClient->clear();
    editPrixApresLivraisonClient->clear();
}

void MainWindow::on_btnEnregistrerClient_clicked()
{
    QString nom = editNomClient->text().trimmed();
    QString email = editEmailClient->text().trimmed();
    QString tel = editTelephoneClient->text().trimmed();
    QString produit = editProduitClient->text().trimmed();
    QString dateCmd = editDateCommandeClient->date().toString("dd/MM/yyyy");
    QString dateLiv = editDateLivraisonClient->date().toString("dd/MM/yyyy");
    QString prixHT = editPrixHTClient->text().trimmed();
    QString modePaie = comboModePaiementClient->currentText();
    QString tva = editTVAClient->text().trimmed();
    QString remise = editRemiseClient->text().trimmed();
    QString prixTTC = editPrixTTCClient->text().trimmed();
    QString avance = editAvanceClient->text().trimmed();
    QString reste = editResteAPayerClient->text().trimmed();

    if (nom.isEmpty() || prixHT.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir au moins le nom du client et le prix HT.");
        return;
    }

    QDate today = QDate::currentDate();
    QDate livDate = editDateLivraisonClient->date();
    QString statut = (livDate <= today) ? "Livrée" : "En cours";

    if (currentRowClient == -1) {
        // Ajout
        int row = tableClients->rowCount();
        tableClients->insertRow(row);

        QString idCmd = "CMD-C-" + QString::number(row + 1);

        tableClients->setItem(row, 0, new QTableWidgetItem(idCmd));
        tableClients->setItem(row, 1, new QTableWidgetItem(nom));
        tableClients->setItem(row, 2, new QTableWidgetItem(email));
        tableClients->setItem(row, 3, new QTableWidgetItem(tel));
        tableClients->setItem(row, 4, new QTableWidgetItem(produit));
        tableClients->setItem(row, 5, new QTableWidgetItem(dateCmd));
        tableClients->setItem(row, 6, new QTableWidgetItem(dateLiv));
        tableClients->setItem(row, 7, new QTableWidgetItem(prixHT));
        tableClients->setItem(row, 8, new QTableWidgetItem(modePaie));
        tableClients->setItem(row, 9, new QTableWidgetItem(statut));
        tableClients->setItem(row, 10, new QTableWidgetItem("1"));
        tableClients->setItem(row, 11, new QTableWidgetItem(prixTTC));

        QMessageBox::information(this, "Succès", "Client ajouté avec succès !");
    } else {
        // Modification
        tableClients->item(currentRowClient, 1)->setText(nom);
        tableClients->item(currentRowClient, 2)->setText(email);
        tableClients->item(currentRowClient, 3)->setText(tel);
        tableClients->item(currentRowClient, 4)->setText(produit);
        tableClients->item(currentRowClient, 5)->setText(dateCmd);
        tableClients->item(currentRowClient, 6)->setText(dateLiv);
        tableClients->item(currentRowClient, 7)->setText(prixHT);
        tableClients->item(currentRowClient, 8)->setText(modePaie);
        tableClients->item(currentRowClient, 9)->setText(statut);
        tableClients->item(currentRowClient, 11)->setText(prixTTC);

        QMessageBox::information(this, "Succès", "Client modifié avec succès !");
    }

    stackedWidgetClients->setCurrentWidget(pageListeClients);
    updateClientStatistics();
    updateClientPerformance();
}

void MainWindow::on_btnModifierClient_clicked()
{
    int row = tableClients->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à modifier.");
        return;
    }

    currentRowClient = row;
    stackedWidgetClients->setCurrentWidget(pageFormulaireClients);

    editIDCommandeClient->setText(tableClients->item(row, 0)->text());
    editNomClient->setText(tableClients->item(row, 1)->text());
    editEmailClient->setText(tableClients->item(row, 2)->text());
    editTelephoneClient->setText(tableClients->item(row, 3)->text());
    editProduitClient->setText(tableClients->item(row, 4)->text());
    editDateCommandeClient->setDate(QDate::fromString(tableClients->item(row, 5)->text(), "dd/MM/yyyy"));
    editDateLivraisonClient->setDate(QDate::fromString(tableClients->item(row, 6)->text(), "dd/MM/yyyy"));
    editPrixHTClient->setText(tableClients->item(row, 7)->text());
    comboModePaiementClient->setCurrentText(tableClients->item(row, 8)->text());
}

void MainWindow::on_btnSupprimerClient_clicked()
{
    int row = tableClients->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation", "Êtes-vous sûr de vouloir supprimer ce client ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        tableClients->removeRow(row);
        QMessageBox::information(this, "Succès", "Client supprimé avec succès !");
        updateClientStatistics();
        updateClientPerformance();
    }
}

void MainWindow::on_btnDetailsClient_clicked()
{
    int row = tableClients->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client pour voir les détails.");
        return;
    }

    stackedWidgetClients->setCurrentWidget(pageDetailsClients);

    detailIdCommandeClient->setText(tableClients->item(row, 0)->text());
    detailNomClient->setText(tableClients->item(row, 1)->text());
    detailEmailClient->setText(tableClients->item(row, 2)->text());
    detailTelephoneClient->setText(tableClients->item(row, 3)->text());
    detailProduitClient->setText(tableClients->item(row, 4)->text());
    detailDateCommandeClient->setText(tableClients->item(row, 5)->text());
    detailDateLivraisonClient->setText(tableClients->item(row, 6)->text());
    detailPrixHTClient->setText(tableClients->item(row, 7)->text() + " DT");
    detailModePaiementClient->setText(tableClients->item(row, 8)->text());
    detailStatutClient->setText(tableClients->item(row, 9)->text());
    detailQteClient->setText(tableClients->item(row, 10)->text());
    detailPrixTTCClient->setText(tableClients->item(row, 11)->text() + " DT");

    detailTVAClient->setText("19%");
    detailRemiseClient->setText("0%");
    detailAvanceClient->setText("0 DT");
    detailResteAPayerClient->setText(tableClients->item(row, 11)->text() + " DT");
}

void MainWindow::on_btnRetourDetailClient_clicked()
{
    stackedWidgetClients->setCurrentWidget(pageListeClients);
}

void MainWindow::searchClient()
{
    QString query = searchClientEdit->text().trimmed().toLower();

    for (int i = 0; i < tableClients->rowCount(); ++i) {
        QString nom = tableClients->item(i, 1)->text().toLower();
        bool match = nom.contains(query);
        tableClients->setRowHidden(i, !match);
    }
}

void MainWindow::sortCommandesClients()
{
    int index = comboSortClients->currentIndex();

    if (index == 0) {
        tableClients->sortItems(1, Qt::AscendingOrder);
    } else if (index == 1) {
        tableClients->sortItems(1, Qt::DescendingOrder);
    } else if (index == 2) {
        tableClients->sortItems(5, Qt::DescendingOrder);
    }
}

void MainWindow::exportFactureClientPDF()
{
    int row = tableClients->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une commande client.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Enregistrer facture",
                                                    "Facture_Client_" + tableClients->item(row, 1)->text() + ".pdf",
                                                    "PDF (*.pdf)");

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));

    QString html = "<html><head><style>"
                   "body { font-family: Arial; margin: 40px; }"
                   "h1 { color: #556b2f; text-align: center; }"
                   "table { width: 100%; border-collapse: collapse; margin-top: 20px; }"
                   "td, th { border: 1px solid #ddd; padding: 12px; text-align: left; }"
                   "th { background-color: #f0f0f0; font-weight: bold; }"
                   ".total { font-size: 18px; font-weight: bold; color: #556b2f; }"
                   "</style></head><body>";

    html += "<h1>FACTURE CLIENT</h1>";
    html += "<p><b>Date:</b> " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>";
    html += "<p><b>ID Commande:</b> " + tableClients->item(row, 0)->text() + "</p>";

    html += "<table>";
    html += "<tr><th>Client</th><td>" + tableClients->item(row, 1)->text() + "</td></tr>";
    html += "<tr><th>Email</th><td>" + tableClients->item(row, 2)->text() + "</td></tr>";
    html += "<tr><th>Téléphone</th><td>" + tableClients->item(row, 3)->text() + "</td></tr>";
    html += "<tr><th>Produit</th><td>" + tableClients->item(row, 4)->text() + "</td></tr>";
    html += "<tr><th>Date commande</th><td>" + tableClients->item(row, 5)->text() + "</td></tr>";
    html += "<tr><th>Date livraison</th><td>" + tableClients->item(row, 6)->text() + "</td></tr>";
    html += "<tr><th>Prix HT</th><td>" + tableClients->item(row, 7)->text() + " DT</td></tr>";
    html += "<tr><th>TVA</th><td>19%</td></tr>";
    html += "<tr><th class='total'>Prix TTC</th><td class='total'>" + tableClients->item(row, 11)->text() + " DT</td></tr>";
    html += "<tr><th>Mode de paiement</th><td>" + tableClients->item(row, 8)->text() + "</td></tr>";
    html += "<tr><th>Statut</th><td>" + tableClients->item(row, 9)->text() + "</td></tr>";
    html += "</table>";

    html += "</body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Succès", "Facture exportée avec succès !");
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void MainWindow::updateClientStatistics()
{
    int totalClients = tableClients->rowCount();
    int totalCommandes = totalClients;
    int enCours = 0;
    int livrees = 0;

    for (int i = 0; i < tableClients->rowCount(); ++i) {
        QString statut = tableClients->item(i, 9)->text();
        if (statut == "En cours") {
            enCours++;
        } else if (statut == "Livrée") {
            livrees++;
        }
    }

    double tauxLivraison = (totalCommandes > 0) ? (livrees * 100.0 / totalCommandes) : 0.0;

    labelTotalClients->setText("Total clients: " + QString::number(totalClients));
    labelTotalCommandesClients->setText("Total commandes: " + QString::number(totalCommandes));
    labelCommandesEnCoursClients->setText("En cours: " + QString::number(enCours));
    labelCommandesLivreesClients->setText("Livrées: " + QString::number(livrees));
    labelTauxLivraisonClients->setText("Taux livraison: " + QString::number(tauxLivraison, 'f', 1) + "%");

    QMap<QString, int> data;
    int totalCount = 0;

    if (enCours > 0) {
        data["En cours"] = enCours;
        totalCount += enCours;
    }
    if (livrees > 0) {
        data["Livrées"] = livrees;
        totalCount += livrees;
    }

    if (!data.isEmpty()) {
        chartClients->setData(data, totalCount);
    }
}

void MainWindow::updateClientPerformance()
{
    tablePerformanceClients->setRowCount(0);

    QMap<QString, int> commandesTotales;
    QMap<QString, int> commandesLivrees;

    for (int i = 0; i < tableClients->rowCount(); ++i) {
        QString nom = tableClients->item(i, 1)->text();
        QString statut = tableClients->item(i, 9)->text();

        commandesTotales[nom]++;
        if (statut == "Livrée") {
            commandesLivrees[nom]++;
        }
    }

    for (auto it = commandesTotales.begin(); it != commandesTotales.end(); ++it) {
        QString nom = it.key();
        int total = it.value();
        int livrees = commandesLivrees.value(nom, 0);
        double taux = (total > 0) ? (livrees * 100.0 / total) : 0.0;

        int row = tablePerformanceClients->rowCount();
        tablePerformanceClients->insertRow(row);
        tablePerformanceClients->setItem(row, 0, new QTableWidgetItem(nom));
        tablePerformanceClients->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
        tablePerformanceClients->setItem(row, 2, new QTableWidgetItem(QString::number(livrees)));
        tablePerformanceClients->setItem(row, 3, new QTableWidgetItem(QString::number(taux, 'f', 1) + "%"));
    }

    QString meilleur = "-";
    int maxCommandes = 0;
    for (auto it = commandesTotales.begin(); it != commandesTotales.end(); ++it) {
        if (it.value() > maxCommandes) {
            maxCommandes = it.value();
            meilleur = it.key();
        }
    }

    labelMeilleurClient->setText("🏆 Meilleur client: " + meilleur + " (" + QString::number(maxCommandes) + " commandes)");
    labelClientRapide->setText("⚡ Client le plus rapide: " + meilleur);
}

void MainWindow::calculatePrixTTCClient()
{
    double prixHT = editPrixHTClient->text().toDouble();
    double tva = editTVAClient->text().toDouble();
    double remise = editRemiseClient->text().toDouble();

    double prixApresRemise = prixHT * (1 - remise / 100.0);
    double prixTTC = prixApresRemise * (1 + tva / 100.0);

    editPrixTTCClient->setText(QString::number(prixTTC, 'f', 2));
    calculateResteAPayerClient();
}

void MainWindow::calculateResteAPayerClient()
{
    double prixTTC = editPrixTTCClient->text().toDouble();
    double avance = editAvanceClient->text().toDouble();
    double reste = prixTTC - avance;

    editResteAPayerClient->setText(QString::number(reste, 'f', 2));
}

// =======================
// IMPLÉMENTATION - QUIZ
// =======================

void MainWindow::initializeQuiz()
{
    currentQuizIndex = 0;
    quizScore = 0;
    quizHasAnsweredCurrent = false;
    showCurrentQuizQuestion();
}

void MainWindow::showCurrentQuizQuestion()
{
    QVector<QuizQuestion> questions = createOliveraQuiz();

    if (currentQuizIndex >= questions.size()) {
        // Quiz terminé
        quizQuestionLabel->setText("🎉 Quiz terminé !");
        quizOption1->hide();
        quizOption2->hide();
        quizOption3->hide();
        btnQuizNext->hide();
        btnQuizRestart->show();

        double pourcentage = (quizScore * 100.0) / questions.size();
        QString message = "Votre score: " + QString::number(quizScore) + "/" + QString::number(questions.size()) +
                          " (" + QString::number(pourcentage, 'f', 0) + "%)";

        QString emoji = "🌟";
        QString commentaire = "Excellent !";
        if (pourcentage < 40) {
            emoji = "📚";
            commentaire = "Continuez à apprendre !";
        } else if (pourcentage < 70) {
            emoji = "👍";
            commentaire = "Bon travail !";
        }

        quizFeedbackLabel->setText(emoji + " " + message + "\n" + commentaire);
        quizFeedbackLabel->setStyleSheet(
            "QLabel { "
            "font-size: 16px; "
            "color: #2c3e50; "
            "padding: 20px; "
            "background-color: #e8f5e9; "
            "border-radius: 8px; "
            "font-weight: bold; "
            "}"
            );
        quizFeedbackLabel->show();
        return;
    }

    QuizQuestion q = questions[currentQuizIndex];

    quizProgressLabel->setText("Question " + QString::number(currentQuizIndex + 1) + " / " + QString::number(questions.size()));
    quizQuestionLabel->setText(q.question);

    quizOption1->setText(q.options[0]);
    quizOption2->setText(q.options[1]);
    quizOption3->setText(q.options[2]);

    quizOption1->setAutoExclusive(false);
    quizOption2->setAutoExclusive(false);
    quizOption3->setAutoExclusive(false);
    quizOption1->setChecked(false);
    quizOption2->setChecked(false);
    quizOption3->setChecked(false);
    quizOption1->setAutoExclusive(true);
    quizOption2->setAutoExclusive(true);
    quizOption3->setAutoExclusive(true);

    quizOption1->show();
    quizOption2->show();
    quizOption3->show();
    btnQuizNext->show();
    btnQuizRestart->hide();
    quizFeedbackLabel->hide();
    quizHasAnsweredCurrent = false;
}

void MainWindow::handleQuizNext()
{
    QVector<QuizQuestion> questions = createOliveraQuiz();

    if (currentQuizIndex >= questions.size()) return;

    if (!quizHasAnsweredCurrent) {
        // Vérifier la réponse
        int selectedAnswer = -1;
        if (quizOption1->isChecked()) selectedAnswer = 0;
        else if (quizOption2->isChecked()) selectedAnswer = 1;
        else if (quizOption3->isChecked()) selectedAnswer = 2;

        if (selectedAnswer == -1) {
            QMessageBox::warning(this, "Attention", "Veuillez sélectionner une réponse avant de continuer.");
            return;
        }

        QuizQuestion q = questions[currentQuizIndex];
        bool correct = (selectedAnswer == q.correctIndex);

        if (correct) {
            quizScore++;
            quizFeedbackLabel->setText("✅ Correct ! " + q.explanation);
            quizFeedbackLabel->setStyleSheet(
                "QLabel { "
                "font-size: 13px; "
                "color: #2e7d32; "
                "padding: 12px; "
                "background-color: #e8f5e9; "
                "border-radius: 6px; "
                "border-left: 4px solid #4caf50; "
                "}"
                );
        } else {
            quizFeedbackLabel->setText("❌ Incorrect. " + q.explanation);
            quizFeedbackLabel->setStyleSheet(
                "QLabel { "
                "font-size: 13px; "
                "color: #c62828; "
                "padding: 12px; "
                "background-color: #ffebee; "
                "border-radius: 6px; "
                "border-left: 4px solid #f44336; "
                "}"
                );
        }

        quizFeedbackLabel->show();
        btnQuizNext->setText("Question suivante");
        quizHasAnsweredCurrent = true;
    } else {
        // Passer à la question suivante
        currentQuizIndex++;
        btnQuizNext->setText("Valider");
        showCurrentQuizQuestion();
    }
}

void MainWindow::handleQuizRestart()
{
    initializeQuiz();
}

// =======================
// IMPLÉMENTATION - GESTION DE STOCK (PRODUCTION)
// =======================

void MainWindow::clearFieldsStock()
{
    editIdentifiantStock->clear();
    editDateProductionStock->setDate(QDate::currentDate());
    comboTypeProduitStock->setCurrentIndex(0);
    editQuantiteMatiereStock->clear();
    editQuantiteProduiteStock->clear();
    editRendementStock->clear();
    editLotProductionStock->clear();
    comboQualiteStock->setCurrentIndex(0);
    editDateExpirationStock->setDate(QDate::currentDate().addYears(2));
}

void MainWindow::updateTableRowStock(int row)
{
    if (row < 0 || row >= tableProductions->rowCount()) return;

    tableProductions->item(row, 1)->setText(editIdentifiantStock->text());
    tableProductions->item(row, 2)->setText(editDateProductionStock->date().toString("dd/MM/yyyy"));
    tableProductions->item(row, 3)->setText(comboTypeProduitStock->currentText());
    tableProductions->item(row, 4)->setText(editQuantiteMatiereStock->text());
    tableProductions->item(row, 5)->setText(editQuantiteProduiteStock->text());
    tableProductions->item(row, 6)->setText(editRendementStock->text());
    tableProductions->item(row, 7)->setText(editLotProductionStock->text());
    tableProductions->item(row, 8)->setText(comboQualiteStock->currentText());
}

void MainWindow::on_btnListeStock_clicked()
{
    // All sections are now always visible, no navigation needed
    genererStatistiquesStock();
}

void MainWindow::on_btnAjouterStock_clicked()
{
    currentRowStock = -1;
    clearFieldsStock();
}

void MainWindow::on_btnAjouterListeStock_clicked()
{
    currentRowStock = -1;
    clearFieldsStock();
}

void MainWindow::on_btnEnregistrerStock_clicked()
{
    QString identifiant = editIdentifiantStock->text().trimmed();
    QDate dateProduction = editDateProductionStock->date();
    QString quantiteMatiere = editQuantiteMatiereStock->text().trimmed();
    QString quantiteProduite = editQuantiteProduiteStock->text().trimmed();
    QString lotProduction = editLotProductionStock->text().trimmed();
    QString typeProduit = comboTypeProduitStock->currentText();

    // Validation
    if (identifiant.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un identifiant de production.");
        return;
    }

    if (quantiteMatiere.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir la quantité de matière première.");
        return;
    }
    
    // For non-Olive products, quantity produced is required
    if (typeProduit != "Olive" && quantiteProduite.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir la quantité produite.");
        return;
    }

    bool okMatiere, okProduite;
    double qteMatiere = quantiteMatiere.toDouble(&okMatiere);
    double qteProduite = 0.0;
    
    if (typeProduit != "Olive") {
        qteProduite = quantiteProduite.toDouble(&okProduite);
    } else {
        okProduite = true; // Skip validation for Olive
    }

    if (!okMatiere || qteMatiere <= 0) {
        QMessageBox::warning(this, "Erreur", "La quantité de matière première doit être un nombre positif.");
        return;
    }

    if (typeProduit != "Olive" && (!okProduite || qteProduite <= 0)) {
        QMessageBox::warning(this, "Erreur", "La quantité produite doit être un nombre positif.");
        return;
    }

    if (dateProduction > QDate::currentDate()) {
        QMessageBox::warning(this, "Erreur", "La date de production ne peut pas être dans le futur.");
        return;
    }

    if (editDateExpirationStock->date() <= dateProduction) {
        QMessageBox::warning(this, "Erreur", "La date d'expiration doit être postérieure à la date de production.");
        return;
    }
    
    // Calculate yield if not already calculated (only for non-Olive products)
    if (typeProduit != "Olive" && editRendementStock->text().isEmpty()) {
        on_btnCalculerRendementStock_clicked();
    }
    
    // EDIT MODE
    if (currentRowStock >= 0) {
        updateTableRowStock(currentRowStock);
        QMessageBox::information(this, "Succès", "Production modifiée avec succès!");
    }
    // ADD MODE
    else {
        int row = tableProductions->rowCount();
        tableProductions->insertRow(row);

        tableProductions->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        tableProductions->setItem(row, 1, new QTableWidgetItem(identifiant));
        tableProductions->setItem(row, 2, new QTableWidgetItem(dateProduction.toString("dd/MM/yyyy")));
        tableProductions->setItem(row, 3, new QTableWidgetItem(typeProduit));
        tableProductions->setItem(row, 4, new QTableWidgetItem(quantiteMatiere));
        // For Olive, quantity produced is empty, for others use entered value
        QString qteProduiteText = (typeProduit == "Olive") ? "-" : quantiteProduite;
        tableProductions->setItem(row, 5, new QTableWidgetItem(qteProduiteText));
        // For Olive, rendement is empty, for others use calculated value
        QString rendementText = (typeProduit == "Olive") ? "-" : editRendementStock->text();
        tableProductions->setItem(row, 6, new QTableWidgetItem(rendementText));
        tableProductions->setItem(row, 7, new QTableWidgetItem(lotProduction));
        // For Olive, qualité is empty, for others use selected value
        QString qualiteText = (typeProduit == "Olive") ? "-" : comboQualiteStock->currentText();
        tableProductions->setItem(row, 8, new QTableWidgetItem(qualiteText));

        // Update search combo if new type
        if (comboRechercheTypeStock->findText(typeProduit) == -1) {
            comboRechercheTypeStock->addItem(typeProduit);
        }

        QMessageBox::information(this, "Succès", "Production ajoutée avec succès!");
    }

    // Reset and update statistics
    currentRowStock = -1;
    clearFieldsStock();
    genererStatistiquesStock();
}

void MainWindow::on_btnModifierStock_clicked()
{
    int row = tableProductions->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une production.");
        return;
    }

    currentRowStock = row;
    editIdentifiantStock->setText(tableProductions->item(row, 1)->text());
    editDateProductionStock->setDate(QDate::fromString(tableProductions->item(row, 2)->text(), "dd/MM/yyyy"));
    
    // Find and set type produit
    QString typeProduit = tableProductions->item(row, 3)->text();
    int index = comboTypeProduitStock->findText(typeProduit);
    if (index >= 0) comboTypeProduitStock->setCurrentIndex(index);
    
    editQuantiteMatiereStock->setText(tableProductions->item(row, 4)->text());
    
    // Set quantity produced (handle "-" for Olive)
    QString qteProduite = tableProductions->item(row, 5)->text();
    if (qteProduite == "-") {
        editQuantiteProduiteStock->clear();
    } else {
        editQuantiteProduiteStock->setText(qteProduite);
    }
    
    // Set rendement (handle "-" for Olive)
    QString rendement = tableProductions->item(row, 6)->text();
    if (rendement == "-") {
        editRendementStock->clear();
    } else {
        editRendementStock->setText(rendement);
    }
    
    editLotProductionStock->setText(tableProductions->item(row, 7)->text());
    
    // Find and set qualité (handle "-" for Olive)
    QString qualite = tableProductions->item(row, 8)->text();
    if (qualite == "-") {
        comboQualiteStock->setCurrentIndex(0);
    } else {
        index = comboQualiteStock->findText(qualite);
        if (index >= 0) comboQualiteStock->setCurrentIndex(index);
    }

    // Set expiration date (default to 2 years from production date)
    QDate prodDate = QDate::fromString(tableProductions->item(row, 2)->text(), "dd/MM/yyyy");
    editDateExpirationStock->setDate(prodDate.addYears(2));
    
    // Trigger the type change handler to update UI state
    on_comboTypeProduitStock_currentIndexChanged(0);
}

void MainWindow::on_btnSupprimerStock_clicked()
{
    int row = tableProductions->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une production.");
        return;
    }

    if (QMessageBox::question(this, "Supprimer",
                              "Voulez-vous vraiment supprimer cette production ?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        tableProductions->removeRow(row);
        
        // Update IDs
        for (int i = row; i < tableProductions->rowCount(); ++i) {
            tableProductions->item(i, 0)->setText(QString::number(i + 1));
        }
        
        // Update statistics
        genererStatistiquesStock();
    }
}

void MainWindow::on_btnDetailsStock_clicked()
{
    int row = tableProductions->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une production.");
        return;
    }

    // Get data directly from table
    QString id = tableProductions->item(row, 0)->text();
    QString identifiant = tableProductions->item(row, 1)->text();
    QString dateProduction = tableProductions->item(row, 2)->text();
    QString typeProduit = tableProductions->item(row, 3)->text();
    QString quantiteMatiere = tableProductions->item(row, 4)->text();
    QString quantiteProduite = tableProductions->item(row, 5)->text();
    QString rendement = tableProductions->item(row, 6)->text();
    QString lot = tableProductions->item(row, 7)->text();
    QString qualite = tableProductions->item(row, 8)->text();
    
    // Calculate expiration date (default to 2 years from production date)
    QDate prodDate = QDate::fromString(dateProduction, "dd/MM/yyyy");
    QString dateExpiration = prodDate.addYears(2).toString("dd/MM/yyyy");

    // Show details in a message box
    QString details = QString("ID: %1\nIdentifiant: %2\nDate production: %3\nType: %4\n"
                             "Qte matière: %5 KG\nQte produite: %6 L\nRendement: %7%\n"
                             "Lot: %8\nQualité: %9\nDate expiration: %10")
                      .arg(id)
                      .arg(identifiant)
                      .arg(dateProduction)
                      .arg(typeProduit)
                      .arg(quantiteMatiere)
                      .arg(quantiteProduite)
                      .arg(rendement)
                      .arg(lot)
                      .arg(qualite)
                      .arg(dateExpiration);
    
    QMessageBox::information(this, "Détails de la production", details);
}

void MainWindow::on_btnCalculerRendementStock_clicked()
{
    QString typeProduit = comboTypeProduitStock->currentText();
    
    // Don't calculate yield for Olive
    if (typeProduit == "Olive") {
        return;
    }
    
    QString quantiteMatiere = editQuantiteMatiereStock->text().trimmed();
    QString quantiteProduite = editQuantiteProduiteStock->text().trimmed();

    if (quantiteMatiere.isEmpty() || quantiteProduite.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir les quantités de matière première et produite pour calculer le rendement.");
        return;
    }

    bool okMatiere, okProduite;
    double qteMatiere = quantiteMatiere.toDouble(&okMatiere);
    double qteProduite = quantiteProduite.toDouble(&okProduite);

    if (!okMatiere || qteMatiere <= 0) {
        QMessageBox::warning(this, "Erreur", "La quantité de matière première doit être un nombre positif.");
        return;
    }

    if (!okProduite || qteProduite <= 0) {
        QMessageBox::warning(this, "Erreur", "La quantité produite doit être un nombre positif.");
        return;
    }

    // Calculate yield: quantité matière première en KG / quantité produite en L
    double rendement = qteMatiere / qteProduite;
    editRendementStock->setText(QString::number(rendement, 'f', 2));
}

void MainWindow::on_comboRechercheTypeStock_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    filtrerParTypeStock();
}

void MainWindow::on_comboTriStock_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    trierTableauStock();
}

void MainWindow::on_comboTypeProduitStock_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    QString typeProduit = comboTypeProduitStock->currentText();
    
    if (typeProduit == "Olive") {
        // Hide quantity produced field and label
        labelQuantiteProduiteStock->setVisible(false);
        editQuantiteProduiteStock->setVisible(false);
        
        // Disable rendement field and button
        editRendementStock->setEnabled(false);
        editRendementStock->setReadOnly(true);
        editRendementStock->clear();
        btnCalculerRendementStock->setEnabled(false);
        
        // Disable qualité field
        comboQualiteStock->setEnabled(false);
        comboQualiteStock->setCurrentIndex(0);
    } else {
        // Show quantity produced field and label
        labelQuantiteProduiteStock->setVisible(true);
        editQuantiteProduiteStock->setVisible(true);
        
        // Change label back to LITRES
        labelQuantiteProduiteStock->setText("Quantité produite (LITRES):");
        
        // Enable rendement field and button
        editRendementStock->setEnabled(true);
        editRendementStock->setReadOnly(true); // Still read-only, but enabled for calculation
        btnCalculerRendementStock->setEnabled(true);
        
        // Enable qualité field
        comboQualiteStock->setEnabled(true);
    }
}

void MainWindow::filtrerParTypeStock()
{
    QString typeFiltre = comboRechercheTypeStock->currentText();
    
    if (typeFiltre == "Tous") {
        // Show all rows
        for (int i = 0; i < tableProductions->rowCount(); ++i) {
            tableProductions->setRowHidden(i, false);
        }
    } else {
        // Hide rows that don't match the filter
        for (int i = 0; i < tableProductions->rowCount(); ++i) {
            QTableWidgetItem* item = tableProductions->item(i, 3);
            if (item) {
                QString typeProduit = item->text();
                tableProductions->setRowHidden(i, typeProduit != typeFiltre);
            } else {
                tableProductions->setRowHidden(i, true);
            }
        }
    }
}

void MainWindow::trierTableauStock()
{
    QString triSelection = comboTriStock->currentText();
    
    // Disable automatic sorting temporarily
    tableProductions->setSortingEnabled(false);
    
    // Get all rows data
    QList<QList<QTableWidgetItem*>> rows;
    for (int i = 0; i < tableProductions->rowCount(); ++i) {
        QList<QTableWidgetItem*> row;
        for (int j = 0; j < tableProductions->columnCount(); ++j) {
            QTableWidgetItem* item = tableProductions->item(i, j);
            if (item) {
                row.append(new QTableWidgetItem(*item));
            } else {
                row.append(new QTableWidgetItem(""));
            }
        }
        rows.append(row);
    }
    
    // Sort based on selection
    if (triSelection == "Date (croissant)") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem*>& a, const QList<QTableWidgetItem*>& b) {
            if (a.size() <= 2 || b.size() <= 2) return false;
            QDate dateA = QDate::fromString(a[2]->text(), "dd/MM/yyyy");
            QDate dateB = QDate::fromString(b[2]->text(), "dd/MM/yyyy");
            if (!dateA.isValid()) return false;
            if (!dateB.isValid()) return true;
            return dateA < dateB;
        });
    } else if (triSelection == "Date (décroissant)") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem*>& a, const QList<QTableWidgetItem*>& b) {
            if (a.size() <= 2 || b.size() <= 2) return false;
            QDate dateA = QDate::fromString(a[2]->text(), "dd/MM/yyyy");
            QDate dateB = QDate::fromString(b[2]->text(), "dd/MM/yyyy");
            if (!dateA.isValid()) return false;
            if (!dateB.isValid()) return true;
            return dateA > dateB;
        });
    } else if (triSelection == "Type produit") {
        std::sort(rows.begin(), rows.end(), [](const QList<QTableWidgetItem*>& a, const QList<QTableWidgetItem*>& b) {
            if (a.size() <= 3 || b.size() <= 3) return false;
            return a[3]->text() < b[3]->text();
        });
    }
    
    // Clear and repopulate table
    tableProductions->setRowCount(0);
    for (int i = 0; i < rows.size(); ++i) {
        int row = tableProductions->rowCount();
        tableProductions->insertRow(row);
        for (int j = 0; j < rows[i].size(); ++j) {
            tableProductions->setItem(row, j, rows[i][j]);
        }
        // Update ID
        tableProductions->item(row, 0)->setText(QString::number(i + 1));
    }
    
    // Re-enable sorting
    tableProductions->setSortingEnabled(true);
    
    // Update statistics after sorting
    genererStatistiquesStock();
}

void MainWindow::genererStatistiquesStock()
{
    QMap<QString, int> statsCount;
    int totalProductions = 0;
    
    // Count productions by type
    for (int i = 0; i < tableProductions->rowCount(); ++i) {
        if (!tableProductions->isRowHidden(i)) {
            QString typeProduit = tableProductions->item(i, 3)->text();
            
            // Use the type directly as category
            QString category = typeProduit;
            
            // Normalize category names
            if (category.contains("Huile d'olive", Qt::CaseInsensitive)) {
                category = "Huile d'olive";
            } else if (category.contains("Huile végétale", Qt::CaseInsensitive) || 
                      category.contains("Huile vegetale", Qt::CaseInsensitive)) {
                category = "Huile végétale";
            } else if (category.contains("Olive", Qt::CaseInsensitive)) {
                category = "Olive";
            }
            
            statsCount[category]++;
            totalProductions++;
        }
    }
    
    // Update pie chart widget with the categories
    if (pieChartWidgetStock) {
        pieChartWidgetStock->setData(statsCount, totalProductions);
    }
}

void MainWindow::on_btnExportPDFStock_clicked()
{
    exporterPDFStock();
}

void MainWindow::exporterPDFStock()
{
    // Ask for month selection
    QDate currentDate = QDate::currentDate();
    QDate monthStart = QDate(currentDate.year(), currentDate.month(), 1);
    QDate monthEnd = monthStart.addMonths(1).addDays(-1);
    
    // Get all productions for the current month
    QList<QList<QString>> productionsMois;
    double totalQuantiteProduite = 0.0;
    QMap<QString, double> quantiteParType;
    
    for (int i = 0; i < tableProductions->rowCount(); ++i) {
        QString dateStr = tableProductions->item(i, 2)->text();
        QDate dateProd = parseDateFromStringStock(dateStr);
        
        if (dateProd >= monthStart && dateProd <= monthEnd) {
            QList<QString> row;
            for (int j = 0; j < tableProductions->columnCount(); ++j) {
                QTableWidgetItem* item = tableProductions->item(i, j);
                row.append(item ? item->text() : "");
            }
            productionsMois.append(row);
            
            // Calculate totals
            bool ok;
            double qteProduite = row[5].toDouble(&ok);
            if (ok && row[5] != "-") {
                totalQuantiteProduite += qteProduite;
                quantiteParType[row[3]] += qteProduite;
            }
        }
    }
    
    // Ask for file save location
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF",
                                                    QString("Etat_Stock_%1_%2.pdf")
                                                    .arg(currentDate.year())
                                                    .arg(currentDate.month(), 2, 10, QChar('0')),
                                                    "Fichiers PDF (*.pdf)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Create PDF
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    
    // Set page size (Qt6 compatible)
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        printer.setPageSize(QPageSize::A4);
        printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    #else
        printer.setPageSize(QPrinter::A4);
        printer.setPageMargins(20, 20, 20, 20, QPrinter::Millimeter);
    #endif
    
    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int yPos = 50;
    // Get page height (compatible Qt5 and Qt6)
    int resolution = printer.resolution();
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        int pageHeight = static_cast<int>((297.0 - 40.0) * resolution / 25.4);
    #else
        int pageHeight = printer.pageRect().height();
    #endif
    int lineHeight = 20;
    
    // Title
    QFont titleFont("Arial", 16, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(50, yPos, "État de Stock - " + 
                     monthStart.toString("MMMM yyyy"));
    yPos += 40;
    
    // Summary
    QFont normalFont("Arial", 10);
    painter.setFont(normalFont);
    painter.drawText(50, yPos, "Période : " + monthStart.toString("dd/MM/yyyy") + 
                     " - " + monthEnd.toString("dd/MM/yyyy"));
    yPos += 30;
    painter.drawText(50, yPos, "Total quantité produite : " + 
                     QString::number(totalQuantiteProduite, 'f', 2) + " L");
    yPos += 30;
    
    // Table header
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    int xPos = 50;
    QStringList headers = {"ID", "Identifiant", "Date", "Type", "Qte Matière", 
                          "Qte Produite", "Rendement", "Lot", "Qualité"};
    int colWidths[] = {30, 100, 80, 60, 80, 80, 70, 60, 100};
    
    painter.drawRect(50, yPos, 700, lineHeight + 5);
    for (int i = 0; i < headers.size(); ++i) {
        painter.drawText(xPos + 5, yPos + 15, headers[i]);
        xPos += colWidths[i];
    }
    yPos += lineHeight + 10;
    
    // Table rows
    painter.setFont(QFont("Arial", 8));
    for (const QList<QString>& row : productionsMois) {
        if (yPos > pageHeight - 100) {
            printer.newPage();
            yPos = 50;
        }
        
        xPos = 50;
        painter.drawRect(50, yPos, 700, lineHeight + 5);
        for (int i = 0; i < qMin(row.size(), headers.size()); ++i) {
            QString text = row[i];
            if (text.length() > 15) {
                text = text.left(12) + "...";
            }
            painter.drawText(xPos + 5, yPos + 15, text);
            xPos += colWidths[i];
        }
        yPos += lineHeight + 5;
    }
    
    // Summary by type
    yPos += 20;
    if (yPos > pageHeight - 100) {
        printer.newPage();
        yPos = 50;
    }
    
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(50, yPos, "Résumé par type de produit :");
    yPos += 25;
    
    painter.setFont(QFont("Arial", 9));
    for (auto it = quantiteParType.begin(); it != quantiteParType.end(); ++it) {
        double pourcentage = (it.value() * 100.0) / totalQuantiteProduite;
        painter.drawText(50, yPos, it.key() + " : " + 
                        QString::number(it.value(), 'f', 2) + " L (" + 
                        QString::number(pourcentage, 'f', 2) + "%)");
        yPos += 20;
    }
    
    painter.end();
    
    QMessageBox::information(this, "Succès", 
                            "Le fichier PDF a été généré avec succès !\n" + fileName);
}

QDate MainWindow::parseDateFromStringStock(const QString &dateStr)
{
    return QDate::fromString(dateStr, "dd/MM/yyyy");
}
