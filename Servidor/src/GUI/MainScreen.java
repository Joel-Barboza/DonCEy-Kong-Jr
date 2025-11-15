package GUI;

import javax.swing.*;
import java.awt.*;

public class MainScreen {

    private static final Color YELLOW = new Color(255, 163, 0);
    private static final Color BLUE = new Color(0, 104, 163);
    private static final Color RED = new Color(251, 0, 0);

    public MainScreen() {
        JFrame mainFrame = new JFrame();
        mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        mainFrame.setSize(1000, 600);
        mainFrame.setLocationRelativeTo(null);

        JPanel leftPanel = createLeftPanel();
        JPanel rightPanel = createRightPanel();

        leftPanel.setPreferredSize(new Dimension(300, 0)); // 1/3 de una ventana de 900px

        mainFrame.setLayout(new BorderLayout());
        mainFrame.add(leftPanel, BorderLayout.WEST);
        mainFrame.add(rightPanel, BorderLayout.CENTER);

        mainFrame.setVisible(true);
    }


    private JPanel createLeftPanel() {
        JPanel leftPanel = new JPanel();
        leftPanel.setBackground(RED);

        // Center content vertically and horizontally
        leftPanel.setLayout(new GridBagLayout());

        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridx = 0;
        gbc.gridy = GridBagConstraints.RELATIVE;   // automatic vertical stacking
        gbc.insets = new Insets(10, 0, 10, 0);     // spacing between rows
        gbc.anchor = GridBagConstraints.CENTER;

        // Container to stack buttons vertically
        JPanel buttonContainer = new JPanel();
        buttonContainer.setOpaque(false);          // keep background color from parent
        buttonContainer.setLayout(new BoxLayout(buttonContainer, BoxLayout.Y_AXIS));

        // Button style
        Font buttonFont = new Font("Arial", Font.BOLD, 20);    // bigger, bold text
        Dimension buttonSize = new Dimension(220, 50);         // fixed button size

        // Create buttons
        JButton btnNewGame = new JButton("Nueva Partida");
        JButton btnSpectate = new JButton("Espectar");
        JButton btnAdmin = new JButton("Administrar Partidas");

        JButton[] buttons = { btnNewGame, btnSpectate, btnAdmin };

        for (JButton b : buttons) {
            b.setBackground(YELLOW);                            // custom background color
            b.setFont(buttonFont);                              // bold, large font
            b.setPreferredSize(buttonSize);                     // enforce size
            b.setMaximumSize(buttonSize);                       // required for BoxLayout
            b.setForeground(Color.BLACK);                       // black text
            b.setBorder(BorderFactory.createLineBorder(Color.BLACK, 2)); // 2px border
            b.setFocusable(false);                              // removes focus outline
        }

        // Add buttons with spacing between them
        buttonContainer.add(btnNewGame);
        buttonContainer.add(Box.createVerticalStrut(20));       // vertical spacing
        buttonContainer.add(btnSpectate);
        buttonContainer.add(Box.createVerticalStrut(20));
        buttonContainer.add(btnAdmin);

        // Center the container inside the left panel
        leftPanel.add(buttonContainer, gbc);

        return leftPanel;
    }




    private JPanel createRightPanel() {
        JPanel rightPanel = new JPanel(new BorderLayout());
        rightPanel.setBackground(BLUE);

        ImageIcon icon = new ImageIcon("src/GUI/resources/main_screen_img.png");
        Image original = icon.getImage();
        Image scaled = original.getScaledInstance(
                original.getWidth(icon.getImageObserver())/2,
                original.getHeight(icon.getImageObserver())/2, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon = new ImageIcon(scaled);

        JLabel imgLabel = new JLabel(scaledIcon, SwingConstants.CENTER);
        imgLabel.setFont(new Font("Arial", Font.PLAIN, 20));
        imgLabel.setForeground(Color.WHITE);

        rightPanel.add(imgLabel, BorderLayout.CENTER);

        return rightPanel;
    }

    public void createGUI() {}

}
