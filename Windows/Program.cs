using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

class SnakeGame : Form
{
    private System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
    private List<Point> snake = new List<Point>();
    private Point food;
    private int rows = 20, cols = 30, cellSize = 20;
    private string direction = "RIGHT";
    private int score = 0;
    private Image backgroundImage;

    public SnakeGame()
    {
        this.Text = "Dakto INC Snake";
        this.ClientSize = new Size(cols * cellSize, rows * cellSize + 30);
        this.FormBorderStyle = FormBorderStyle.FixedSingle;
        this.MaximizeBox = false;
        this.DoubleBuffered = true;

        snake.Add(new Point(cols / 2, rows / 2));
        PlaceFood();

        timer.Interval = 150;
        timer.Tick += (s, e) => { MoveSnake(); Invalidate(); };
        timer.Start();

        this.KeyDown += new KeyEventHandler(OnKeyDown);
        this.Paint += new PaintEventHandler(OnPaint);

        try
        {
            backgroundImage = Image.FromFile("background.png");
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

    private void OnKeyDown(object sender, KeyEventArgs e)
    {
        switch (e.KeyCode)
        {
            case Keys.W: case Keys.Up: if (direction != "DOWN") direction = "UP"; break;
            case Keys.S: case Keys.Down: if (direction != "UP") direction = "DOWN"; break;
            case Keys.A: case Keys.Left: if (direction != "RIGHT") direction = "LEFT"; break;
            case Keys.D: case Keys.Right: if (direction != "LEFT") direction = "RIGHT"; break;
            case Keys.Q: this.Close(); break;
        }
    }

    private void OnPaint(object sender, PaintEventArgs e)
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
            g.FillRectangle(snakeBrush, p.X * cellSize, p.Y * cellSize, cellSize, cellSize);
        }

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
