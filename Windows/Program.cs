using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

class SnakeGame : Form
{
    private bool showMenu = true;
    private System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
    private List<Point> snake = new List<Point>();
    private Point food;
    private int rows = 20, cols = 30, cellSize = 20;
    private const int minRows = 20, minCols = 30;
    private string direction = "RIGHT";
    private int score = 0;
    private Image? backgroundImage = null;

    public SnakeGame()
    {
        ShowMenu();
        this.Text = "Dakto INC Snake";
    this.ClientSize = new Size(cols * cellSize, rows * cellSize + 30);
    this.FormBorderStyle = FormBorderStyle.Sizable;
    this.MaximizeBox = true;
    this.Resize += new EventHandler(OnResize);
    private void OnResize(object? sender, EventArgs e)
    {
        int availableWidth = this.ClientSize.Width;
        int availableHeight = this.ClientSize.Height - 30;
        cellSize = Math.Min(availableWidth / minCols, availableHeight / minRows);
        cols = Math.Max(minCols, availableWidth / cellSize);
        rows = Math.Max(minRows, availableHeight / cellSize);
        Invalidate();
    }
        this.DoubleBuffered = true;

    this.KeyDown += new KeyEventHandler(OnKeyDown);
    this.Paint += new PaintEventHandler(OnPaint);

        try
        {
            using (var webClient = new System.Net.WebClient())
            {
                using (var stream = new System.IO.MemoryStream(webClient.DownloadData("https://daktoinc.co.uk/background.png")))
                {
                    backgroundImage = Image.FromStream(stream);
                }
            }
        }
        catch
        {
            backgroundImage = null;
        }
    }

    private void PlaceFood()
    {
        Random rand = new Random();
        do
        {
            food = new Point(rand.Next(cols), rand.Next(rows));
        } while (snake.Contains(food));
    }

    private void MoveSnake()
    {
        Point head = snake[0];
        Point newHead = head;

        switch (direction)
        {
            case "UP": newHead = new Point(head.X, head.Y - 1); break;
            case "DOWN": newHead = new Point(head.X, head.Y + 1); break;
            case "LEFT": newHead = new Point(head.X - 1, head.Y); break;
            case "RIGHT": newHead = new Point(head.X + 1, head.Y); break;
        }

        if (newHead.X < 0 || newHead.X >= cols || newHead.Y < 0 || newHead.Y >= rows || snake.Contains(newHead))
    private void ShowMenu()
    {
        showMenu = true;
        Button playButton = new Button();
        playButton.Text = "Play";
        playButton.Font = new Font("Arial", 16, FontStyle.Bold);
        playButton.Size = new Size(200, 60);
        playButton.Location = new Point((this.ClientSize.Width - playButton.Width) / 2, this.ClientSize.Height / 2 - 80);
        playButton.Click += (s, e) => StartGame();
        this.Controls.Add(playButton);

        Button quitButton = new Button();
        quitButton.Text = "Quit";
        quitButton.Font = new Font("Arial", 16, FontStyle.Bold);
        quitButton.Size = new Size(200, 60);
        quitButton.Location = new Point((this.ClientSize.Width - quitButton.Width) / 2, this.ClientSize.Height / 2 + 10);
        quitButton.Click += (s, e) => this.Close();
        this.Controls.Add(quitButton);
    }

    private void HideMenu()
    {
        showMenu = false;
        foreach (Control control in this.Controls)
        {
            if (control is Button)
            {
                this.Controls.Remove(control);
            }
        }
    }

    private void StartGame()
    {
        HideMenu();
        snake.Clear();
        cols = Math.Max(minCols, this.ClientSize.Width / cellSize);
        rows = Math.Max(minRows, (this.ClientSize.Height - 30) / cellSize);
        snake.Add(new Point(cols / 2, rows / 2));
        score = 0;
        direction = "RIGHT";
        PlaceFood();
        timer.Interval = 150;
        timer.Tick -= (s, e) => { MoveSnake(); Invalidate(); };
        timer.Tick += (s, e) => { MoveSnake(); Invalidate(); };
        timer.Start();
    }
        {
            timer.Stop();
            MessageBox.Show($"Game Over! Score: {score}");
            this.Close();
            return;
        }

        snake.Insert(0, newHead);

        if (newHead == food)
        { 
            score++;
            PlaceFood();
        }
        else
        {
            snake.RemoveAt(snake.Count - 1);
        }
    }

    private bool isFullscreen = false;
    private FormBorderStyle prevBorderStyle;
    private Rectangle prevBounds;

    private void OnKeyDown(object? sender, KeyEventArgs e)
    {
        switch (e.KeyCode)
        {
            case Keys.W: case Keys.Up: if (direction != "DOWN") direction = "UP"; break;
            case Keys.S: case Keys.Down: if (direction != "UP") direction = "DOWN"; break;
            case Keys.A: case Keys.Left: if (direction != "RIGHT") direction = "LEFT"; break;
            case Keys.D: case Keys.Right: if (direction != "LEFT") direction = "RIGHT"; break;
            case Keys.Q: this.Close(); break;
            case Keys.F11:
                ToggleFullscreen();
                break;
        }
    }

    private void ToggleFullscreen()
    {
        if (!isFullscreen)
        {
            prevBorderStyle = this.FormBorderStyle;
            prevBounds = this.Bounds;
            this.FormBorderStyle = FormBorderStyle.None;
            this.WindowState = FormWindowState.Maximized;
            isFullscreen = true;
        }
        else
        {
            this.FormBorderStyle = prevBorderStyle;
            this.WindowState = FormWindowState.Normal;
            this.Bounds = prevBounds;
            isFullscreen = false;
        }
    }

    private void OnPaint(object? sender, PaintEventArgs e)
    {
        Graphics g = e.Graphics;
        if (backgroundImage != null)
        {
            g.DrawImage(backgroundImage, 0, 0, cols * cellSize, rows * cellSize);
        }
        else
        {
            g.Clear(Color.Black);
        }

        Brush snakeBrush = Brushes.Lime;
        Brush foodBrush = Brushes.Red;
        Font scoreFont = new Font("Arial", 12, FontStyle.Bold);

        foreach (var p in snake)
        {
            if (p.X < cols && p.Y < rows)
                g.FillRectangle(snakeBrush, p.X * cellSize, p.Y * cellSize, cellSize, cellSize);
        }

        if (food.X < cols && food.Y < rows)
            g.FillEllipse(foodBrush, food.X * cellSize, food.Y * cellSize, cellSize, cellSize);

        g.DrawString($"Score: {score}", scoreFont, Brushes.White, new PointF(5, rows * cellSize));
    }

    [STAThread]
    static void Main()
    {
        Application.EnableVisualStyles();
        Application.Run(new SnakeGame());
    }
}
