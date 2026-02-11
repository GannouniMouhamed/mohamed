#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMap>
#include <QColor>
#include <QList>

class QStackedWidget;
class QWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QSpinBox;
class QDoubleSpinBox;
class QDateEdit;
class QLabel;
class QCheckBox;
class QRadioButton;
class QSplitter;
class QPaintEvent;

// Pie chart widget (inlined here so we only need main window files)
class PieChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PieChartWidget(QWidget *parent = nullptr);
    void setData(const QMap<QString, int> &data, int total);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<QString, int> m_data;
    int m_total;
    QList<QColor> m_colors;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation principales
    void openGestionEmployes();
    void openChangePassword();
    void backToMenu();
    void openGestionFournisseurs();
    void openGestionClients();
    void openGestionStocks();
    void openQuiz();

    // Mot de passe
    void changePassword();

    // Employés
    void showAjouter();
    void showModifier();
    void enregistrer();
    void supprimer();
    void tableSelectionChanged();
    void searchByName();
    void sortBySalary();
    void extractAttestation();

    // Fournisseurs
    void on_btnListe_clicked();
    void on_btnAjouter_clicked();
    void on_btnEnregistrer_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnDetails_clicked();
    void on_btnRetourDetail_clicked();

    // Fournisseurs - nouvelles fonctionnalités
    void searchFournisseur();
    void sortCommandesParNom();
    void exportFacturePDF();
    void updateFournisseurStatistics();
    void updatePerformanceMetrics();
    void calculatePrixTTC();
    void calculateResteAPayer();

    // Clients
    void on_btnListeClients_clicked();
    void on_btnAjouterClient_clicked();
    void on_btnEnregistrerClient_clicked();
    void on_btnModifierClient_clicked();
    void on_btnSupprimerClient_clicked();
    void on_btnDetailsClient_clicked();
    void on_btnRetourDetailClient_clicked();
    void searchClient();
    void sortCommandesClients();
    void exportFactureClientPDF();
    void updateClientStatistics();
    void updateClientPerformance();
    void calculatePrixTTCClient();
    void calculateResteAPayerClient();

    // Quiz
    void handleQuizNext();
    void handleQuizRestart();

    // Gestion de Stock (Production)
    void on_btnListeStock_clicked();
    void on_btnAjouterStock_clicked();
    void on_btnAjouterListeStock_clicked();
    void on_btnEnregistrerStock_clicked();
    void on_btnModifierStock_clicked();
    void on_btnSupprimerStock_clicked();
    void on_btnDetailsStock_clicked();
    void on_btnCalculerRendementStock_clicked();
    void on_btnExportPDFStock_clicked();
    void on_comboRechercheTypeStock_currentIndexChanged(int index);
    void on_comboTriStock_currentIndexChanged(int index);
    void on_comboTypeProduitStock_currentIndexChanged(int index);

private:
    void setupUI();
    void setupStyle();
    void reindexIds();
    void updateStatistics();
    void initializeQuiz();
    void showCurrentQuizQuestion();
    
    // Gestion de Stock - méthodes privées
    void clearFieldsStock();
    void updateTableRowStock(int row);
    void trierTableauStock();
    void filtrerParTypeStock();
    void genererStatistiquesStock();
    void exporterPDFStock();
    QDate parseDateFromStringStock(const QString &dateStr);

    // Navigation générale
    QStackedWidget *mainStack;
    QWidget *pageLogin;
    QWidget *pageMenu;
    QWidget *pageChangePassword;
    QWidget *pageEmployes;
    QWidget *pageFournisseurs;
    QWidget *pageClients;
    QWidget *pageStocks;
    QWidget *pageQuiz;

    // Login
    QLineEdit *editUser;
    QLineEdit *editPass;
    QPushButton *btnLogin;

    // Menu principal
    QPushButton *btnGestionEmployes;
    QPushButton *btnGestionClients;
    QPushButton *btnGestionStocks;
    QPushButton *btnGestionVentes;
    QPushButton *btnFournisseur;
    QPushButton *btnModifMotDePasse;
    QPushButton *btnQuiz;

    // Changement mot de passe
    QLineEdit *editOldPass;
    QLineEdit *editNewPass;
    QLineEdit *editConfirmPass;
    QPushButton *btnSavePass;
    QPushButton *btnBackPass;
    QString currentPassword = "admin";

    // Employés - liste
    QLineEdit *editSearch;
    QComboBox *comboSort;
    QTableWidget *tableEmployes;
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnExtractionAttestation;

    // Employés - formulaire + stats
    QLabel *chartTitle;
    PieChartWidget *chartWidget;
    QLabel *formTitle;
    QLineEdit *editNom;
    QLineEdit *editPrenom;
    QLineEdit *editPoste;
    QLineEdit *editTelephone;
    QLineEdit *editEmail;
    QSpinBox *spinSalaire;
    QSpinBox *spinHeures;
    QDateEdit *dateEmbauche;
    QDateEdit *dateNaissance;
    QPushButton *btnEnregistrer;

    int selectedRow = -1;

    // Fournisseurs - menu latéral
    QPushButton *btnListeFournisseurs;
    QPushButton *btnAjouterFournisseur;
    QPushButton *btnRetourMenuFournisseurs;

    // Fournisseurs - stacked widget
    QStackedWidget *stackedWidgetFournisseurs;
    QWidget *pageListeFournisseurs;
    QWidget *pageFormulaireFournisseurs;
    QWidget *pageDetailsFournisseurs;

    // Fournisseurs - liste
    QTableWidget *tableFournisseurs;
    QPushButton *btnModifierFournisseur;
    QPushButton *btnSupprimerFournisseur;
    QPushButton *btnDetailsFournisseur;

    // Fournisseurs - recherche et tri
    QLineEdit *searchFournisseurEdit;
    QComboBox *comboSortFournisseurs;
    QPushButton *btnExportPDF;

    // Fournisseurs - statistiques
    QLabel *labelTotalFournisseurs;
    QLabel *labelTotalCommandes;
    QLabel *labelCommandesEnCours;
    QLabel *labelCommandesLivrees;
    QLabel *labelTauxLivraison;
    PieChartWidget *chartFournisseurs;

    // Fournisseurs - performance
    QTableWidget *tablePerformance;
    QLabel *labelMeilleurFournisseur;
    QLabel *labelFournisseurRapide;

    // Fournisseurs - formulaire
    QLineEdit *editNomFournisseur;
    QLineEdit *editEmailFournisseur;
    QLineEdit *editTelephoneFournisseur;
    QLineEdit *editProduitFournisseur;
    QPushButton *btnEnregistrerFournisseur;

    // Commandes fournisseurs - champs
    QLineEdit *editIDCommande;
    QDateEdit *editDateCommande;
    QDateEdit *editDateLivraison;
    QLineEdit *editPrixHT;
    QComboBox *comboModePaiement;
    QLineEdit *editAvance;
    QLineEdit *editResteAPayer;
    QLineEdit *editTVA;
    QLineEdit *editRemise;
    QLineEdit *editPrixTTC;
    QLineEdit *editPrixApresLivraison;

    // Fournisseurs - détails
    QLineEdit *detailIdCommande;
    QLineEdit *detailNom;
    QLineEdit *detailEmail;
    QLineEdit *detailTelephone;
    QLineEdit *detailProduit;
    QLineEdit *detailDateCommande;
    QLineEdit *detailDateLivraison;
    QLineEdit *detailPrixHT;
    QLineEdit *detailModePaiement;
    QLineEdit *detailAvance;
    QLineEdit *detailResteAPayer;
    QLineEdit *detailTVA;
    QLineEdit *detailRemise;
    QLineEdit *detailPrixTTC;
    QLineEdit *detailStatut;
    QLineEdit *detailQte;
    QLabel *detailsTitle;
    QPushButton *btnRetourDetail;

    int currentRowFournisseur = -1;

    // CLIENTS - menu latéral
    QPushButton *btnListeClients;
    QPushButton *btnAjouterClient;
    QPushButton *btnRetourMenuClients;

    // CLIENTS - stacked widget
    QStackedWidget *stackedWidgetClients;
    QWidget *pageListeClients;
    QWidget *pageFormulaireClients;
    QWidget *pageDetailsClients;

    // CLIENTS - liste
    QTableWidget *tableClients;
    QPushButton *btnModifierClient;
    QPushButton *btnSupprimerClient;
    QPushButton *btnDetailsClient;

    // CLIENTS - recherche et tri
    QLineEdit *searchClientEdit;
    QComboBox *comboSortClients;
    QPushButton *btnExportPDFClient;

    // CLIENTS - statistiques
    QLabel *labelTotalClients;
    QLabel *labelTotalCommandesClients;
    QLabel *labelCommandesEnCoursClients;
    QLabel *labelCommandesLivreesClients;
    QLabel *labelTauxLivraisonClients;
    PieChartWidget *chartClients;

    // CLIENTS - performance
    QTableWidget *tablePerformanceClients;
    QLabel *labelMeilleurClient;
    QLabel *labelClientRapide;

    // CLIENTS - formulaire
    QLineEdit *editNomClient;
    QLineEdit *editEmailClient;
    QLineEdit *editTelephoneClient;
    QLineEdit *editProduitClient;
    QPushButton *btnEnregistrerClient;

    // Commandes clients - champs
    QLineEdit *editIDCommandeClient;
    QDateEdit *editDateCommandeClient;
    QDateEdit *editDateLivraisonClient;
    QLineEdit *editPrixHTClient;
    QComboBox *comboModePaiementClient;
    QLineEdit *editAvanceClient;
    QLineEdit *editResteAPayerClient;
    QLineEdit *editTVAClient;
    QLineEdit *editRemiseClient;
    QLineEdit *editPrixTTCClient;
    QLineEdit *editPrixApresLivraisonClient;

    // CLIENTS - détails
    QLineEdit *detailIdCommandeClient;
    QLineEdit *detailNomClient;
    QLineEdit *detailEmailClient;
    QLineEdit *detailTelephoneClient;
    QLineEdit *detailProduitClient;
    QLineEdit *detailDateCommandeClient;
    QLineEdit *detailDateLivraisonClient;
    QLineEdit *detailPrixHTClient;
    QLineEdit *detailModePaiementClient;
    QLineEdit *detailAvanceClient;
    QLineEdit *detailResteAPayerClient;
    QLineEdit *detailTVAClient;
    QLineEdit *detailRemiseClient;
    QLineEdit *detailPrixTTCClient;
    QLineEdit *detailStatutClient;
    QLineEdit *detailQteClient;
    QLabel *detailsTitleClient;
    QPushButton *btnRetourDetailClient;

    int currentRowClient = -1;

    // GESTION DE STOCK (Production)
    QWidget *pageListeStock;
    
    // Stock - liste
    QTableWidget *tableProductions;
    QPushButton *btnModifierStock;
    QPushButton *btnSupprimerStock;
    QPushButton *btnDetailsStock;
    QPushButton *btnAjouterListeStock;
    QPushButton *btnExportPDFStock;
    
    // Stock - recherche et tri
    QComboBox *comboRechercheTypeStock;
    QComboBox *comboTriStock;
    
    // Stock - statistiques
    PieChartWidget *pieChartWidgetStock;
    QWidget *pieChartContainerStock;
    
    // Stock - formulaire
    QLineEdit *editIdentifiantStock;
    QDateEdit *editDateProductionStock;
    QComboBox *comboTypeProduitStock;
    QLineEdit *editQuantiteMatiereStock;
    QLineEdit *editQuantiteProduiteStock;
    QLineEdit *editRendementStock;
    QLineEdit *editLotProductionStock;
    QComboBox *comboQualiteStock;
    QDateEdit *editDateExpirationStock;
    QPushButton *btnEnregistrerStock;
    QPushButton *btnCalculerRendementStock;
    QLabel *labelQuantiteProduiteStock;
    
    // Stock - splitter
    QSplitter *mainSplitterStock;
    QSplitter *rightSplitterStock;
    QWidget *sectionListeStock;
    QWidget *sectionStatistiquesStock;
    QWidget *sectionFormulaireStock;
    
    int currentRowStock = -1;

    // Quiz - OLIVERAQ
    QLabel *quizTitle;
    QLabel *quizIntro;
    QLabel *quizQuestionLabel;
    QRadioButton *quizOption1;
    QRadioButton *quizOption2;
    QRadioButton *quizOption3;
    QLabel *quizProgressLabel;
    QLabel *quizFeedbackLabel;
    QPushButton *btnQuizNext;
    QPushButton *btnQuizRestart;
    QPushButton *btnQuizBackToMenu;
    int currentQuizIndex = 0;
    int quizScore = 0;
    bool quizHasAnsweredCurrent = false;
};

#endif // MAINWINDOW_H
