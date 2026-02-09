import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read CSV file
df = pd.read_csv('frame_stats.csv')

# Create figure with subplots (2x2 layout)
fig = plt.figure(figsize=(14, 10))
fig.suptitle('Profiler Metrics Visualization', fontsize=16, fontweight='bold')

# 1. Frame Time Over Time (Total and Work)
ax1 = plt.subplot(2, 2, 1)
ax1.plot(df['Frame'], df['Total(ms)'], label='Total Frame Time', linewidth=1.5, alpha=1.0)
ax1.plot(df['Frame'], df['Work(ms)'], label='Work Time', linewidth=1.5, alpha=0.8)
ax1.axhline(y=16.67, color='r', linestyle='--', label='60 FPS Target (16.67ms)', alpha=0.7)
ax1.set_xlabel('Frame')
ax1.set_ylabel('Time (ms)')
ax1.set_title('Frame Time Over Time')
ax1.legend()
ax1.grid(True, alpha=0.3)

# 2. FPS Over Time (rolling average over 60 frames)
ax2 = plt.subplot(2, 2, 2)
window_size = 60  # 60 frames ≈ 1 second at 60 FPS
rolling_avg_time = df['Total(ms)'].rolling(window=window_size, min_periods=1).mean()
rolling_fps = 1000.0 / rolling_avg_time
ax2.plot(df['Frame'], rolling_fps, label=f'FPS (rolling {window_size} frames)', linewidth=1.5, color='green', alpha=0.8)
ax2.axhline(y=60, color='r', linestyle='--', label='60 FPS Target', alpha=0.7)
ax2.set_xlabel('Frame')
ax2.set_ylabel('FPS')
ax2.set_title('FPS Over Time (Rolling Average)')
ax2.legend()
ax2.grid(True, alpha=0.3)

# 3. Section Breakdown (Stacked Area)
ax3 = plt.subplot(2, 2, 3)
sections = ['Input(ms)', 'AI(ms)', 'Physics(ms)', 'Render(ms)']
colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A']
ax3.stackplot(df['Frame'], df['Input(ms)'], df['AI(ms)'], df['Physics(ms)'], df['Render(ms)'],
              labels=['Input', 'AI', 'Physics', 'Render'],
              colors=colors, alpha=0.7)
ax3.set_xlabel('Frame')
ax3.set_ylabel('Time (ms)')
ax3.set_title('Section Breakdown (Stacked)')
ax3.legend(loc='upper left')
ax3.grid(True, alpha=0.3)

# 4. Section Averages (Bar Chart)
ax4 = plt.subplot(2, 2, 4)
section_avgs = [df['Input(ms)'].mean(), df['AI(ms)'].mean(), 
                df['Physics(ms)'].mean(), df['Render(ms)'].mean()]
section_names = ['Input', 'AI', 'Physics', 'Render']
bars = ax4.bar(section_names, section_avgs, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
ax4.set_ylabel('Average Time (ms)')
ax4.set_title('Average Section Times')
ax4.grid(True, alpha=0.3, axis='y')
# Add value labels on bars
for bar in bars:
    height = bar.get_height()
    ax4.text(bar.get_x() + bar.get_width()/2., height,
             f'{height:.2f}ms', ha='center', va='bottom', fontsize=10, fontweight='bold')


plt.tight_layout()
plt.savefig('profiler_metrics.png', dpi=300, bbox_inches='tight')
print("Graphs saved to 'profiler_metrics.png'")

# Print summary statistics
print("\n=== Summary Statistics ===")
print(f"Total Frames: {len(df)}")
print(f"Average Frame Time: {df['Total(ms)'].mean():.2f} ms")
print(f"Average FPS: {rolling_fps.mean():.2f}")
print(f"Min Frame Time: {df['Total(ms)'].min():.2f} ms")
print(f"Max Frame Time: {df['Total(ms)'].max():.2f} ms")
print(f"\nSection Averages:")
for name, avg in zip(section_names, section_avgs):
    print(f"  {name}: {avg:.2f} ms")
